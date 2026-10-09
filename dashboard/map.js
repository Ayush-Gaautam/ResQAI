let map;
let ambulanceRoute = null;
let hospitalRoute = null;
let ambulanceMarker = null;
let emergencyMarker = null;
let hospitalMarker = null;
let selectedMarker = null;

const ambulanceIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/2967/2967350.png",
    iconSize: [38, 38],
    iconAnchor: [19, 38],
    popupAnchor: [0, -30]
});
const emergencyIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/684/684908.png",
    iconSize: [38, 38],
    iconAnchor: [19, 38],
    popupAnchor: [0, -30]
});
const hospitalIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/2966/2966488.png",
    iconSize: [38, 38],
    iconAnchor: [19, 38],
    popupAnchor: [0, -30]
});

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
    ambulanceRoute = drawRoute(result.ambulance_route, "green", ambulanceRoute);
    hospitalRoute = drawRoute(result.hospital_route, "blue", hospitalRoute);
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
