// ============================================
// ResQAI Dashboard
// script.js (Part 1)
// ============================================

// CSV File Paths
const ambulanceCSV = "../outputs/ambulance_to_patient.csv";
const hospitalCSV = "../outputs/patient_to_hospital.csv";

// Route Data
let ambulanceRoutePoints = [];
let hospitalRoutePoints = [];

// ============================================
// Load Dashboard
// ============================================

document.addEventListener("DOMContentLoaded", () => {

    loadRoutes();

});

// ============================================
// Load Both CSV Files
// ============================================

async function loadRoutes() {

    try {

        // Read Ambulance Route
        const ambulanceResponse = await fetch(ambulanceCSV);

        if (!ambulanceResponse.ok) {
            throw new Error("Cannot load ambulance_to_patient.csv");
        }

        const ambulanceText = await ambulanceResponse.text();

        ambulanceRoutePoints = parseCSV(ambulanceText);

        // Read Hospital Route
        const hospitalResponse = await fetch(hospitalCSV);

        if (!hospitalResponse.ok) {
            throw new Error("Cannot load patient_to_hospital.csv");
        }

        const hospitalText = await hospitalResponse.text();

        hospitalRoutePoints = parseCSV(hospitalText);

        console.log("Ambulance Route:", ambulanceRoutePoints);

        console.log("Hospital Route:", hospitalRoutePoints);

        // Draw Everything
        displayRoutes();

    }

    catch (error) {

        console.error(error);

        alert("Unable to load CSV files.");

    }

}

// ============================================
// Convert CSV → Coordinates
// ============================================

function parseCSV(csvText) {

    const lines = csvText.trim().split("\n");

    const coordinates = [];

    // Skip Header
    for (let i = 1; i < lines.length; i++) {

        const values = lines[i].split(",");

        if (values.length < 3)
            continue;

        const latitude = parseFloat(values[1]);

        const longitude = parseFloat(values[2]);

        if (!isNaN(latitude) && !isNaN(longitude)) {

            coordinates.push([latitude, longitude]);

        }

    }

    return coordinates;

}

// ============================================
// Display Everything
// ============================================

function displayRoutes() {

    if (ambulanceRoutePoints.length === 0 ||
        hospitalRoutePoints.length === 0) {

        alert("No Route Found.");

        return;

    }

    // Draw Ambulance Route
    drawAmbulanceRoute(ambulanceRoutePoints);

    // Draw Hospital Route
    drawHospitalRoute(hospitalRoutePoints);

    // Add Ambulance Marker
    addAmbulanceMarker(
        ambulanceRoutePoints[0]
    );

    // Emergency Marker
    addEmergencyMarker(
        ambulanceRoutePoints[
            ambulanceRoutePoints.length - 1
        ]
    );

    // Hospital Marker
    addHospitalMarker(
        hospitalRoutePoints[
            hospitalRoutePoints.length - 1
        ]
    );

    // Zoom Map
    zoomToRoutes();
    updateDashboard();
    replaceMarkers();
    highlightRoutes();

}
// ============================================
// PART 2
// Update Sidebar Information
// ============================================

function updateDashboard() {

    // Emergency Node
    document.getElementById("emergencyNode").innerHTML =
        "813348627";

    // Ambulance ID
    document.getElementById("ambulanceId").innerHTML =
        "AMB-001";

    // Hospital Name
    document.getElementById("hospitalName").innerHTML =
        "PGI Chandigarh";

    // Distance
    let distance = calculateDistance();

    document.getElementById("distance").innerHTML =
        distance.toFixed(2) + " m";

    // ETA
    let eta = calculateETA(distance);

    document.getElementById("eta").innerHTML =
        eta.toFixed(1) + " min";

    // Traffic
    document.getElementById("trafficStatus").innerHTML =
        "Normal";
}


// ============================================
// Distance Calculation
// ============================================

function calculateDistance() {

    let total = 0;

    for(let i=1;i<hospitalRoutePoints.length;i++){

        const p1 = hospitalRoutePoints[i-1];
        const p2 = hospitalRoutePoints[i];

        total += haversineDistance(
            p1[0],
            p1[1],
            p2[0],
            p2[1]
        );

    }

    return total;

}


// ============================================
// Haversine Formula
// ============================================

function haversineDistance(lat1, lon1, lat2, lon2){

    const R = 6371000;

    const dLat =
        (lat2-lat1) * Math.PI / 180;

    const dLon =
        (lon2-lon1) * Math.PI / 180;

    const a =

        Math.sin(dLat/2) *
        Math.sin(dLat/2)

        +

        Math.cos(lat1*Math.PI/180)

        *

        Math.cos(lat2*Math.PI/180)

        *

        Math.sin(dLon/2)

        *

        Math.sin(dLon/2);

    const c =

        2 *

        Math.atan2(
            Math.sqrt(a),
            Math.sqrt(1-a)
        );

    return R*c;

}


// ============================================
// ETA
// ============================================

function calculateETA(distance){

    // Assume 40 km/hr

    const speed = 40000/60;

    return distance/speed;

}


// ============================================
// Call Dashboard Update
// ============================================

updateDashboard();

// ============================================
// PART 3
// Professional Features
// ============================================

// Ambulance Icon
const ambulanceIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/2967/2967350.png",
    iconSize: [38,38],
    iconAnchor: [19,38],
    popupAnchor: [0,-30]
});

// Emergency Icon
const emergencyIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/684/684908.png",
    iconSize: [38,38],
    iconAnchor: [19,38],
    popupAnchor: [0,-30]
});

// Hospital Icon
const hospitalIcon = L.icon({
    iconUrl: "https://cdn-icons-png.flaticon.com/512/2966/2966488.png",
    iconSize: [38,38],
    iconAnchor: [19,38],
    popupAnchor: [0,-30]
});


// ============================================
// Replace Default Markers
// ============================================

function replaceMarkers(){

    if(ambulanceMarker)
        map.removeLayer(ambulanceMarker);

    if(emergencyMarker)
        map.removeLayer(emergencyMarker);

    if(hospitalMarker)
        map.removeLayer(hospitalMarker);


    ambulanceMarker = L.marker(
        ambulanceRoutePoints[0],
        {icon:ambulanceIcon}
    )
    .addTo(map)
    .bindPopup(
        "<b>🚑 Ambulance</b><br>" +
        "ID : AMB-001<br>" +
        "Status : Available"
    );


    emergencyMarker = L.marker(
        ambulanceRoutePoints[
            ambulanceRoutePoints.length-1
        ],
        {icon:emergencyIcon}
    )
    .addTo(map)
    .bindPopup(
        "<b>🚨 Emergency</b><br>" +
        "Node : 813348627"
    );


    hospitalMarker = L.marker(
        hospitalRoutePoints[
            hospitalRoutePoints.length-1
        ],
        {icon:hospitalIcon}
    )
    .addTo(map)
    .bindPopup(
        "<b>🏥 Hospital</b><br>" +
        "PGI Chandigarh"
    );

}


// ============================================
// Highlight Routes
// ============================================

function highlightRoutes(){

    ambulanceRoute.setStyle({

        color:"green",

        weight:8,

        opacity:1

    });

    hospitalRoute.setStyle({

        color:"blue",

        weight:8,

        opacity:1

    });

}


// ============================================
// Refresh Dashboard
// ============================================

function refreshDashboard(){

    ambulanceRoutePoints=[];

    hospitalRoutePoints=[];

    loadRoutes();

}


// ============================================
// Keyboard Shortcut
// ============================================

document.addEventListener("keydown",function(e){

    if(e.key==="r" || e.key==="R"){

        refreshDashboard();

    }

});


// ============================================
// Finish
// ============================================

replaceMarkers();

highlightRoutes();

console.log("ResQAI Dashboard Loaded Successfully.");