
const apiBaseUrl = (
    window.RESQAI_API_URL || "/api"
).replace(/\/+$/, "");

let selectedLocation = null;

function setText(id, value) {
    const element = document.getElementById(id);
    if (element) {
        element.textContent = value ?? "N/A";
    }
}

function updateDashboard(result) {
    setText("totalRequests", "24");
    setText("pendingRequests", "4");
    setText("acceptedRequests", "18");
    setText("rejectedRequests", "2");
    setText("emergencyNode", result.emergency_node_id);
    setText("ambulanceId", result.ambulance_id);
    setText("ambulanceLocation", result.ambulance_route?.[0]
        ? `Node ${result.ambulance_route[0].node_id}` : "Assigned unit");
    setText("hospitalName", result.hospital && result.hospital.name
        ? result.hospital.name : "N/A");

    setText(
        "distance",
        Number.isFinite(Number(result.total_distance_meters))
            ? `${Number(result.total_distance_meters).toFixed(2)} m`
            : "N/A"
    );

    setText(
        "eta",
        Number.isFinite(Number(result.eta_minutes))
            ? `${Number(result.eta_minutes).toFixed(1)} min`
            : "N/A"
    );

    setText("trafficStatus", result.traffic_status ?? "N/A");
    setText("tripEta", Number.isFinite(Number(result.eta_minutes))
        ? `${Number(result.eta_minutes).toFixed(1)} min` : "N/A");
    setText("trafficSummary", result.traffic_status ?? "N/A");
    setText("fleetCount", result.ambulance_status === "available" ? "Available" : result.ambulance_status);
    setText("tripStatus", result.ambulance_status === "available" ? "En route" : result.ambulance_status);
}

async function requestRoute(payload) {
    const url = `${apiBaseUrl}/route`;

    console.log("ResQAI API request:", url);

    let response;

    try {
        response = await fetch(url, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
        });
    } catch (error) {
        throw new Error(
            `Cannot connect to the API at ${url}. Check the API URL and backend CORS settings.`
        );
    }

    if (!response.ok) {
        let detail = `API request failed (HTTP ${response.status}).`;

        try {
            const data = await response.json();
            if (data.detail) {
                detail = typeof data.detail === "string"
                    ? data.detail
                    : JSON.stringify(data.detail);
            }
        } catch (_) {
            // Keep the HTTP error message.
        }

        throw new Error(detail);
    }

    return response.json();
}

async function generateRoute() {
    const button = document.getElementById("generateRouteBtn");

    if (!button) {
        console.error("Generate Route button was not found.");
        return;
    }

    button.disabled = true;
    button.textContent = "Calculating...";

    try {
        const payload = selectedLocation
            ? {
                latitude: selectedLocation.lat,
                longitude: selectedLocation.lng
            }
            : {
                node_id: 813348627
            };

        const result = await requestRoute(payload);

        if (typeof displayRoute === "function") {
            displayRoute(result);
        } else {
            console.warn("displayRoute() was not found.");
        }

        updateDashboard(result);
        setText("status", "Route ready. Click the map to choose another emergency.");
    } catch (error) {
        console.error("ResQAI route error:", error);
        setText("status", error.message || "Unable to generate route.");
    } finally {
        button.disabled = false;
        button.textContent = "Generate Route";
    }
}

document.addEventListener("DOMContentLoaded", async () => {
    if (typeof initializeMap === "function") {
        initializeMap((latlng) => {
            selectedLocation = latlng;

            const latitude = document.getElementById("latitude");
            const longitude = document.getElementById("longitude");

            if (latitude) {
                latitude.value = latlng.lat.toFixed(6);
            }

            if (longitude) {
                longitude.value = latlng.lng.toFixed(6);
            }

            setText("status", "Location selected. Generate a route.");
        });
    } else {
        console.error("initializeMap() was not found.");
    }

    document
        .getElementById("generateRouteBtn")
        .addEventListener("click", generateRoute);

    document.getElementById("refreshBtn").addEventListener("click", generateRoute);
    document.getElementById("newRequestBtn").addEventListener("click", () => {
        setText("status", "Select an emergency point on the map.");
        document.getElementById("map").scrollIntoView({ behavior: "smooth", block: "center" });
    });

    await generateRoute();
});
