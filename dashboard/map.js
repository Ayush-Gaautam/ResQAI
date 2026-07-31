// ==========================================
// ResQAI Dashboard
// map.js
// ==========================================

// Global map object
let map;

// Route layers
let ambulanceRoute = null;
let hospitalRoute = null;

// Markers
let ambulanceMarker = null;
let emergencyMarker = null;
let hospitalMarker = null;

// Initialize Map
function initializeMap() {

    // Chandigarh Coordinates
    const chandigarh = [30.7333, 76.7794];

    map = L.map("map").setView(chandigarh, 13);

    // OpenStreetMap Tiles
    L.tileLayer(
        "https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png",
        {
            maxZoom: 19,
            attribution:
                "&copy; OpenStreetMap Contributors"
        }
    ).addTo(map);
}

// ==========================================
// Draw Ambulance Route
// ==========================================

function drawAmbulanceRoute(routePoints) {

    if (ambulanceRoute != null) {
        map.removeLayer(ambulanceRoute);
    }

    ambulanceRoute = L.polyline(routePoints, {
        color: "green",
        weight: 6,
        opacity: 0.9
    }).addTo(map);
}

// ==========================================
// Draw Hospital Route
// ==========================================

function drawHospitalRoute(routePoints) {

    if (hospitalRoute != null) {
        map.removeLayer(hospitalRoute);
    }

    hospitalRoute = L.polyline(routePoints, {
        color: "blue",
        weight: 6,
        opacity: 0.9
    }).addTo(map);
}

// ==========================================
// Add Ambulance Marker
// ==========================================

function addAmbulanceMarker(position) {

    if (ambulanceMarker != null) {
        map.removeLayer(ambulanceMarker);
    }

    ambulanceMarker = L.marker(position)
        .addTo(map)
        .bindPopup("🚑 Ambulance");
}

// ==========================================
// Add Emergency Marker
// ==========================================

function addEmergencyMarker(position) {

    if (emergencyMarker != null) {
        map.removeLayer(emergencyMarker);
    }

    emergencyMarker = L.marker(position)
        .addTo(map)
        .bindPopup("🚨 Emergency");
}

// ==========================================
// Add Hospital Marker
// ==========================================

function addHospitalMarker(position) {

    if (hospitalMarker != null) {
        map.removeLayer(hospitalMarker);
    }

    hospitalMarker = L.marker(position)
        .addTo(map)
        .bindPopup("🏥 Hospital");
}

// ==========================================
// Zoom to All Routes
// ==========================================

function zoomToRoutes() {

    let group = [];

    if (ambulanceRoute)
        group.push(ambulanceRoute);

    if (hospitalRoute)
        group.push(hospitalRoute);

    if (group.length > 0) {

        const featureGroup = L.featureGroup(group);

        map.fitBounds(featureGroup.getBounds(), {
            padding: [40, 40]
        });

    }
}

// ==========================================
// Initialize when page loads
// ==========================================

window.onload = function () {

    initializeMap();

};
let selectedMarker = null;

map.on("click", function(e){

    if(selectedMarker)
        map.removeLayer(selectedMarker);

    selectedMarker = L.marker(e.latlng).addTo(map);

    document.getElementById("latitude").value =
        e.latlng.lat.toFixed(6);

    document.getElementById("longitude").value =
        e.latlng.lng.toFixed(6);

});