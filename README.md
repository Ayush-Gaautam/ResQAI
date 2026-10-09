# ResQAI

## AI-Powered Multi-Incident Emergency Response Optimization System

### Problem Statement

Current emergency response systems usually dispatch the nearest available ambulance to each incident independently. During multiple simultaneous emergencies, this approach can lead to inefficient resource allocation and longer overall response times.

### Proposed Solution

ResQAI is an AI-powered decision support system that predicts emergency hotspots using historical accident data and optimally assigns emergency vehicles using graph algorithms and optimization techniques.

### Objectives

- Predict emergency hotspots
- Optimize ambulance allocation
- Compute shortest routes
- Simulate multiple emergency scenarios
- Visualize emergency response

## Tech Stack

### Languages
- C++
- Python

### Algorithms
- Dijkstra Algorithm
- A* Algorithm
- Hungarian Algorithm

### Machine Learning
- Random Forest
- XGBoost

### Visualization
- Folium
- OpenStreetMap

## Project Status

🚧 Under Development

## Backend architecture

The production API is a FastAPI service in `backend/app.py`. It loads the processed
road graph, hospitals, ambulances, and traffic data once at startup, then exposes
validated JSON endpoints for routing. FastAPI also provides interactive API
documentation at `/docs`.

Operational endpoints:

- `GET /health` - liveness and loaded-data counts
- `GET /ready` - readiness for traffic routing
- `GET /metrics` - request, latency-header, and route-cache counters
- `POST /route` - select an emergency node or coordinate and calculate a route
- `GET /hospitals?limit=50&offset=0` - paginated hospital data

The service uses a bounded per-process LRU cache for repeated route requests.
`RESQAI_ROUTE_CACHE_SIZE` and `RESQAI_ROUTE_CACHE_TTL_SECONDS` control its size
and TTL. For multiple Render instances, use a shared Redis cache and a managed
database for mutable ambulance state; local in-memory state should remain only a
performance optimization. Run multiple stateless API instances behind a load
balancer, use `/ready` for deployment health checks, and send logs to a central
logging platform using the `X-Request-ID` response header for request tracing.