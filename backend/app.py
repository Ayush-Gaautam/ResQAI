from __future__ import annotations

import csv
import heapq
import logging
import math
import os
import threading
import time
import uuid
from collections import OrderedDict
from dataclasses import dataclass
from pathlib import Path
from typing import Annotated

from fastapi import FastAPI, HTTPException, Query, Request
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel, Field, model_validator


ROOT = Path(__file__).resolve().parents[1]
DATA_DIR = Path(os.getenv("RESQAI_DATA_DIR", ROOT / "data" / "processed"))
TRAFFIC_FILE = Path(os.getenv("RESQAI_TRAFFIC_FILE", ROOT / "data" / "traffic.csv"))
logger = logging.getLogger("resqai.api")
logging.basicConfig(
    level=os.getenv("RESQAI_LOG_LEVEL", "INFO").upper(),
    format="%(asctime)s %(levelname)s %(name)s request_id=%(request_id)s %(message)s",
)


@dataclass(frozen=True)
class Node:
    latitude: float
    longitude: float


@dataclass(frozen=True)
class Edge:
    destination: int
    cost: float


class RouteRequest(BaseModel):
    node_id: int | None = Field(default=None, gt=0)
    latitude: float | None = Field(default=None, ge=-90, le=90)
    longitude: float | None = Field(default=None, ge=-180, le=180)

    @model_validator(mode="after")
    def require_location(self) -> "RouteRequest":
        if self.node_id is None and (self.latitude is None or self.longitude is None):
            raise ValueError("Provide node_id or both latitude and longitude")
        return self


class RoutePoint(BaseModel):
    node_id: int
    latitude: float
    longitude: float


class RouteResponse(BaseModel):
    emergency_node_id: int
    ambulance_id: str
    ambulance_status: str
    hospital: dict[str, object]
    ambulance_route: list[RoutePoint]
    hospital_route: list[RoutePoint]
    ambulance_distance_meters: float
    hospital_distance_meters: float
    total_distance_meters: float
    eta_minutes: float
    traffic_status: str


class RouteCache:
    def __init__(self, max_entries: int, ttl_seconds: float) -> None:
        self.max_entries = max(1, max_entries)
        self.ttl_seconds = max(1.0, ttl_seconds)
        self._entries: OrderedDict[tuple[object, ...], tuple[float, RouteResponse]] = OrderedDict()
        self._lock = threading.Lock()
        self.hits = 0
        self.misses = 0

    def get(self, key: tuple[object, ...]) -> RouteResponse | None:
        now = time.monotonic()
        with self._lock:
            entry = self._entries.get(key)
            if entry is None:
                self.misses += 1
                return None
            created, response = entry
            if now - created >= self.ttl_seconds:
                del self._entries[key]
                self.misses += 1
                return None
            self._entries.move_to_end(key)
            self.hits += 1
            return response

    def set(self, key: tuple[object, ...], response: RouteResponse) -> None:
        with self._lock:
            self._entries[key] = (time.monotonic(), response)
            self._entries.move_to_end(key)
            while len(self._entries) > self.max_entries:
                self._entries.popitem(last=False)

    def stats(self) -> dict[str, object]:
        with self._lock:
            total = self.hits + self.misses
            return {
                "entries": len(self._entries),
                "max_entries": self.max_entries,
                "ttl_seconds": self.ttl_seconds,
                "hits": self.hits,
                "misses": self.misses,
                "hit_rate": round(self.hits / total, 3) if total else 0,
            }


class RoutingData:
    def __init__(self) -> None:
        self.nodes: dict[int, Node] = {}
        self.graph: dict[int, list[Edge]] = {}
        self.hospitals: list[dict[str, object]] = []
        self.ambulances: list[dict[str, object]] = []
        self._load()

    def _load(self) -> None:
        nodes_file = DATA_DIR / "nodes.csv"
        edges_file = DATA_DIR / "edges.csv"
        hospitals_file = DATA_DIR / "hospitals.csv"
        ambulances_file = DATA_DIR / "ambulances.csv"
        for path in (nodes_file, edges_file, hospitals_file, ambulances_file, TRAFFIC_FILE):
            if not path.exists():
                raise RuntimeError(f"Required routing data is missing: {path}")

        with nodes_file.open(newline="", encoding="utf-8") as file:
            for row in csv.DictReader(file):
                self.nodes[int(row["node_id"])] = Node(
                    latitude=float(row["latitude"]),
                    longitude=float(row["longitude"]),
                )

        traffic: dict[tuple[int, int], tuple[float, bool]] = {}
        with TRAFFIC_FILE.open(newline="", encoding="utf-8") as file:
            for row in csv.DictReader(file):
                multiplier = float(row["multiplier"])
                traffic[(int(row["source"]), int(row["destination"]))] = (
                    multiplier,
                    multiplier == 0 or row["status"].strip().lower() == "closed",
                )

        with edges_file.open(newline="", encoding="utf-8") as file:
            for row in csv.DictReader(file):
                source = int(row["source"])
                destination = int(row["destination"])
                multiplier, blocked = traffic.get((source, destination), (1.0, False))
                if not blocked and destination in self.nodes:
                    self.graph.setdefault(source, []).append(
                        Edge(destination, float(row["length"]) * multiplier)
                    )

        with hospitals_file.open(newline="", encoding="utf-8") as file:
            self.hospitals = [
                {
                    "node_id": int(row["nodeId"]),
                    "name": row["name"],
                    "latitude": float(row["latitude"]),
                    "longitude": float(row["longitude"]),
                }
                for row in csv.DictReader(file)
                if int(row["nodeId"]) in self.nodes
            ]

        with ambulances_file.open(newline="", encoding="utf-8") as file:
            self.ambulances = [
                {
                    "ambulance_id": row["ambulanceId"],
                    "node_id": int(row["nodeId"]),
                    "status": row["status"].strip(),
                }
                for row in csv.DictReader(file)
                if int(row["nodeId"]) in self.nodes
            ]

        if not self.nodes or not self.graph or not self.hospitals:
            raise RuntimeError("Routing data loaded but is incomplete")

    def nearest_node(self, latitude: float, longitude: float) -> int:
        return min(
            self.nodes,
            key=lambda node_id: (self.nodes[node_id].latitude - latitude) ** 2
            + (self.nodes[node_id].longitude - longitude) ** 2,
        )

    def shortest_path(self, source: int, destinations: set[int] | None = None) -> tuple[dict[int, float], dict[int, int]]:
        if source not in self.nodes:
            raise HTTPException(status_code=400, detail=f"Unknown node_id: {source}")
        distances = {source: 0.0}
        parents: dict[int, int] = {}
        queue = [(0.0, source)]
        while queue:
            distance, current = heapq.heappop(queue)
            if distance != distances.get(current):
                continue
            if destinations and current in destinations and len(
                destinations.intersection(distances)
            ) == len(destinations):
                break
            for edge in self.graph.get(current, []):
                candidate = distance + edge.cost
                if candidate < distances.get(edge.destination, math.inf):
                    distances[edge.destination] = candidate
                    parents[edge.destination] = current
                    heapq.heappush(queue, (candidate, edge.destination))
        return distances, parents

    def reconstruct(self, source: int, destination: int, parents: dict[int, int]) -> list[int]:
        if destination != source and destination not in parents:
            return []
        path = [destination]
        while path[-1] != source:
            path.append(parents[path[-1]])
        path.reverse()
        return path

    def points(self, path: list[int]) -> list[RoutePoint]:
        return [
            RoutePoint(node_id=node_id, latitude=self.nodes[node_id].latitude, longitude=self.nodes[node_id].longitude)
            for node_id in path
        ]


app = FastAPI(title="ResQAI Routing API", version="1.0.0")
allowed_origins = [
    origin.strip()
    for origin in os.getenv("RESQAI_ALLOWED_ORIGINS", "*").split(",")
    if origin.strip()
]
app.add_middleware(
    CORSMiddleware,
    allow_origins=allowed_origins,
    allow_credentials=False,
    allow_methods=["GET", "POST", "OPTIONS"],
    allow_headers=["*"],
)
data = RoutingData()
route_cache = RouteCache(
    max_entries=int(os.getenv("RESQAI_ROUTE_CACHE_SIZE", "256")),
    ttl_seconds=float(os.getenv("RESQAI_ROUTE_CACHE_TTL_SECONDS", "300")),
)
request_metrics = {
    "total_requests": 0,
    "successful_requests": 0,
    "failed_requests": 0,
    "route_requests": 0,
}
metrics_lock = threading.Lock()


@app.middleware("http")
async def request_observability(request: Request, call_next):
    request_id = request.headers.get("X-Request-ID", str(uuid.uuid4()))
    started = time.perf_counter()
    with metrics_lock:
        request_metrics["total_requests"] += 1
        if request.url.path == "/route" or request.url.path == "/route/demo":
            request_metrics["route_requests"] += 1
    try:
        response = await call_next(request)
    except Exception:
        with metrics_lock:
            request_metrics["failed_requests"] += 1
        logger.exception("Unhandled request failure method=%s path=%s", request.method, request.url.path, extra={"request_id": request_id})
        raise
    duration_ms = round((time.perf_counter() - started) * 1000, 2)
    if response.status_code >= 400:
        with metrics_lock:
            request_metrics["failed_requests"] += 1
    else:
        with metrics_lock:
            request_metrics["successful_requests"] += 1
    response.headers["X-Request-ID"] = request_id
    response.headers["X-Process-Time-Ms"] = str(duration_ms)
    logger.info(
        "request method=%s path=%s status=%s duration_ms=%s",
        request.method,
        request.url.path,
        response.status_code,
        duration_ms,
        extra={"request_id": request_id},
    )
    return response


@app.get("/health")
def health() -> dict[str, object]:
    return {
        "status": "ok",
        "nodes": len(data.nodes),
        "hospitals": len(data.hospitals),
        "ambulances": len(data.ambulances),
        "cache": route_cache.stats(),
    }


@app.get("/ready")
def readiness() -> dict[str, object]:
    return {
        "status": "ready",
        "service": "resqai-routing-api",
        "data_loaded": bool(data.nodes and data.graph and data.hospitals),
    }


@app.get("/")
def api_info() -> dict[str, object]:
    return {
        "service": "ResQAI Routing API",
        "version": app.version,
        "docs": "/docs",
        "health": "/health",
        "readiness": "/ready",
        "route": "POST /route",
    }


@app.get("/metrics")
def metrics() -> dict[str, object]:
    with metrics_lock:
        request_snapshot = dict(request_metrics)
    return {
        "service": "resqai-routing-api",
        "requests": request_snapshot,
        "route_cache": route_cache.stats(),
    }


@app.get("/hospitals")
def hospitals(
    limit: Annotated[int, Query(ge=1, le=100)] = 50,
    offset: Annotated[int, Query(ge=0)] = 0,
) -> dict[str, object]:
    return {
        "items": data.hospitals[offset:offset + limit],
        "total": len(data.hospitals),
        "limit": limit,
        "offset": offset,
    }


@app.post("/route", response_model=RouteResponse)
def route(request: RouteRequest) -> RouteResponse:
    cache_key = (
        "node",
        request.node_id,
    ) if request.node_id is not None else (
        "coordinates",
        round(request.latitude or 0, 5),
        round(request.longitude or 0, 5),
    )
    cached = route_cache.get(cache_key)
    if cached is not None:
        return cached

    response = _calculate_route(request)
    route_cache.set(cache_key, response)
    return response


def _calculate_route(request: RouteRequest) -> RouteResponse:
    emergency_node = request.node_id
    if emergency_node is None:
        emergency_node = data.nearest_node(request.latitude, request.longitude)  # type: ignore[arg-type]
    if emergency_node not in data.nodes:
        raise HTTPException(status_code=400, detail=f"Unknown node_id: {emergency_node}")

    available = [ambulance for ambulance in data.ambulances if ambulance["status"].lower() == "available"]
    if not available:
        raise HTTPException(status_code=503, detail="No available ambulance")

    ambulance_choice: dict[str, object] | None = None
    ambulance_path: list[int] = []
    ambulance_distance = math.inf
    for ambulance in available:
        distances, parents = data.shortest_path(int(ambulance["node_id"]))
        distance = distances.get(emergency_node, math.inf)
        if distance < ambulance_distance:
            path = data.reconstruct(int(ambulance["node_id"]), emergency_node, parents)
            if path:
                ambulance_choice = ambulance
                ambulance_path = path
                ambulance_distance = distance
    if ambulance_choice is None:
        raise HTTPException(status_code=404, detail="No available ambulance can reach the emergency")

    hospital_nodes = {int(hospital["node_id"]) for hospital in data.hospitals}
    distances, parents = data.shortest_path(emergency_node, hospital_nodes)
    reachable = [hospital for hospital in data.hospitals if int(hospital["node_id"]) in distances]
    if not reachable:
        raise HTTPException(status_code=404, detail="No reachable hospital")
    hospital = min(reachable, key=lambda item: distances[int(item["node_id"])])
    hospital_node = int(hospital["node_id"])
    hospital_path = data.reconstruct(emergency_node, hospital_node, parents)
    hospital_distance = distances[hospital_node]
    total_distance = ambulance_distance + hospital_distance
    traffic_status = "Heavy" if any(edge.cost > 2 * 100 for edges in data.graph.values() for edge in edges) else "Normal"
    return RouteResponse(
        emergency_node_id=emergency_node,
        ambulance_id=str(ambulance_choice["ambulance_id"]),
        ambulance_status=str(ambulance_choice["status"]),
        hospital=hospital,
        ambulance_route=data.points(ambulance_path),
        hospital_route=data.points(hospital_path),
        ambulance_distance_meters=round(ambulance_distance, 2),
        hospital_distance_meters=round(hospital_distance, 2),
        total_distance_meters=round(total_distance, 2),
        eta_minutes=round(total_distance / (40000 / 60), 1),
        traffic_status=traffic_status,
    )


@app.get("/route/demo", response_model=RouteResponse)
def demo_route(
    node_id: Annotated[int, Query(gt=0)] = 813348627,
) -> RouteResponse:
    return route(RouteRequest(node_id=node_id))
