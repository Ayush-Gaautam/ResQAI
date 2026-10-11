let map;
let ambulanceRoute = null;
let hospitalRoute = null;
let ambulanceMarker = null;
let emergencyMarker = null;
let hospitalMarker = null;
let selectedMarker = null;

function markerIcon(kind, symbol) {
    return L.divIcon({
        className: "",
        html: `<span class="custom-marker ${kind}-marker">${symbol}</span>`,
        iconSize: [29, 29],
        iconAnchor: [14, 14]
    });
}

const ambulanceIcon = markerIcon("ambulance", "✚");
const emergencyIcon = markerIcon("emergency", "!");
const hospitalIcon = markerIcon("hospital", "✚");

function initializeMap(onSelect) {
    map = L.map("map").setView([30.7333, 76.7794], 13);
    L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
        maxZoom: 19,
        attribution: "&copy; OpenStreetMap Contributors"
    }).addTo(map);
    map.on("click", (event) => {
        if (selectedMarker) map.removeLayer(selectedMarker);
        selectedMarker = L.marker(event.latlng).addTo(map);
        onSelect(event.latlng);
    });
}

function drawRoute(points, color, currentLayer) {
    if (currentLayer) map.removeLayer(currentLayer);
    return L.polyline(points.map((point) => [point.latitude, point.longitude]), {
        color, weight: 7, opacity: 0.9
    }).addTo(map);
}

function displayRoute(result) {
    if (!result.ambulance_route?.length || !result.hospital_route?.length || !result.hospital) {
        throw new Error("The API returned an incomplete route.");
    }
    ambulanceRoute = drawRoute(result.ambulance_route, "#27b47e", ambulanceRoute);
    hospitalRoute = drawRoute(result.hospital_route, "#2f80ed", hospitalRoute);
    [ambulanceMarker, emergencyMarker, hospitalMarker].forEach((marker) => {
        if (marker) map.removeLayer(marker);
    });
    const ambulanceStart = result.ambulance_route[0];
    const emergency = result.hospital_route[0];
    const hospital = result.hospital;
    ambulanceMarker = L.marker([ambulanceStart.latitude, ambulanceStart.longitude], { icon: ambulanceIcon })
        .addTo(map).bindPopup(`<b>Ambulance</b><br>${result.ambulance_id}`);
    emergencyMarker = L.marker([emergency.latitude, emergency.longitude], { icon: emergencyIcon })
        .addTo(map).bindPopup(`<b>Emergency</b><br>Node ${result.emergency_node_id}`);
    hospitalMarker = L.marker([hospital.latitude, hospital.longitude], { icon: hospitalIcon })
        .addTo(map).bindPopup(`<b>Hospital</b><br>${hospital.name}`);
    const bounds = L.featureGroup([ambulanceRoute, hospitalRoute]).getBounds();
    map.fitBounds(bounds, { padding: [40, 40] });
}
