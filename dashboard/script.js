const apiBaseUrl = (window.RESQAI_API_URL || "/api").replace(/\/$/, "");
let selectedLocation = null;

function setText(id, value) {
    document.getElementById(id).textContent = value;
}

function updateDashboard(result) {
    setText("emergencyNode", result.emergency_node_id);
    setText("ambulanceId", result.ambulance_id);
    setText("hospitalName", result.hospital.name);
    setText("distance", `${result.total_distance_meters.toFixed(2)} m`);
    setText("eta", `${result.eta_minutes.toFixed(1)} min`);
    setText("trafficStatus", result.traffic_status);
}

async function requestRoute(payload) {
    const response = await fetch(`${apiBaseUrl}/route`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload)
    });
    if (!response.ok) {
        let detail = "Unable to generate route.";
        try {
            detail = (await response.json()).detail || detail;
        } catch (_) {
            // Keep the user-facing fallback when the server does not return JSON.
        }
        throw new Error(detail);
    }
    return response.json();
}

async function generateRoute() {
    const button = document.getElementById("generateRouteBtn");
    button.disabled = true;
    button.textContent = "Calculating...";
    try {
        const payload = selectedLocation
            ? { latitude: selectedLocation.lat, longitude: selectedLocation.lng }
            : { node_id: 813348627 };
        const result = await requestRoute(payload);
        displayRoute(result);
        updateDashboard(result);
        setText("status", "Route ready. Click the map to choose another emergency.");
    } catch (error) {
        console.error(error);
        setText("status", error.message);
    } finally {
        button.disabled = false;
        button.textContent = "Generate Route";
    }
}

document.addEventListener("DOMContentLoaded", async () => {
    initializeMap((latlng) => {
        selectedLocation = latlng;
        document.getElementById("latitude").value = latlng.lat.toFixed(6);
        document.getElementById("longitude").value = latlng.lng.toFixed(6);
        setText("status", "Location selected. Generate a route.");
    });
    document.getElementById("generateRouteBtn").addEventListener("click", generateRoute);
    await generateRoute();
});
