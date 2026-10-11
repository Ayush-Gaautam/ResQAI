
const apiBaseUrl = (
    window.RESQAI_API_URL || "/api"
).replace(/\/+$/, "");

let selectedLocation = null;
let latestRouteResult = null;
let refreshPromise = null;

async function loadOperationsSummary() {
    const response = await fetch(`${apiBaseUrl}/operations`);
    if (!response.ok) {
        throw new Error(`Operations summary failed (HTTP ${response.status}).`);
    }
    const summary = await response.json();
    const capacity = summary.hospitals.capacity_percent;
    setText("hospitalCapacity", `${capacity}%`);
    setText("hospitalCount", summary.hospitals.operational);
    setText("availableVehicles", summary.fleet.available);
    setText("busyVehicles", summary.fleet.busy);
    setText("totalVehicles", summary.fleet.total);
    setText("optimizationSaving", `${summary.optimization.average_saving_percent}%`);
    setText("optimizationAlgorithm", summary.optimization.algorithm);
    const capacityBar = document.getElementById("capacityBar");
    if (capacityBar) capacityBar.style.width = `${capacity}%`;
    return summary;
}

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
        latestRouteResult = result;
        setText("status", "Route ready. Click the map to choose another emergency.");
        return result;
    } catch (error) {
        console.error("ResQAI route error:", error);
        setText("status", error.message || "Unable to generate route.");
        throw error;
    } finally {
        button.disabled = false;
        button.textContent = "Generate Route";
    }
}

async function refreshData() {
        if (refreshPromise) return refreshPromise;
        const button = document.getElementById("refreshBtn");
        if (button) {
            button.disabled = true;
            button.textContent = "Refreshing…";
        }
        setText("status", "Refreshing route and operations data…");
        refreshPromise = Promise.all([loadOperationsSummary(), generateRoute()])
            .then(() => setText("status", "Data refreshed successfully."))
            .catch((error) => {
                console.error("ResQAI refresh error:", error);
                setText("status", "Refresh failed. Check the API connection and try again.");
            })
            .finally(() => {
                refreshPromise = null;
                if (button) {
                    button.disabled = false;
                    button.textContent = "↻ Refresh data";
                }
            });
        return refreshPromise;
    }

function openModal(id) {
        const backdrop = document.getElementById("modalBackdrop");
        document.querySelectorAll(".demo-modal").forEach((modal) => {
            modal.hidden = modal.id !== id;
        });
        backdrop.hidden = false;
        if (id === "paymentModal") generateDemoQr();
        if (id === "invoiceModal") renderInvoices();
    }

function closeModal() {
        document.getElementById("modalBackdrop").hidden = true;
    }

function generateDemoQr() {
        const reference = `RESQ-DEMO-${Math.random().toString(36).slice(2, 8).toUpperCase()}`;
        setText("demoPaymentReference", reference);
        const fallback = document.getElementById("qrFallback");
        const image = document.getElementById("paymentQr");
        fallback.hidden = true;
        image.hidden = false;
        image.src = `https://api.qrserver.com/v1/create-qr-code/?size=220x220&data=${encodeURIComponent(`ResQAI demo payment reference ${reference}`)}`;
        image.onerror = () => {
            image.hidden = true;
            fallback.hidden = false;
        };
    }

function renderInvoices() {
        const route = latestRouteResult;
        const hospital = route?.hospital?.name || "Nearest hospital";
        const distance = route?.total_distance_meters
            ? `${Number(route.total_distance_meters).toFixed(2)} m`
            : "Pending route";
        document.getElementById("invoiceList").innerHTML = `
            <div class="invoice-row"><div><strong>INV-DEMO-001</strong><small>Dispatch coordination · ${hospital}</small></div><b>Demo record</b></div>
            <div class="invoice-row"><div><strong>INV-DEMO-002</strong><small>Route service · ${distance}</small></div><b>Demo record</b></div>`;
    }

function addChatMessage(text, type) {
        const message = document.createElement("div");
        message.className = `chat-message ${type}`;
        message.textContent = text;
        document.getElementById("chatThread").appendChild(message);
        message.scrollIntoView({ block: "nearest" });
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

    document.getElementById("refreshBtn").addEventListener("click", refreshData);
    document.getElementById("newRequestBtn").addEventListener("click", () => {
        setText("status", "Select an emergency point on the map.");
        document.getElementById("map").scrollIntoView({ behavior: "smooth", block: "center" });
    });
    document.getElementById("paymentsNav").addEventListener("click", (event) => {
        event.preventDefault();
        openModal("paymentModal");
    });
    document.getElementById("invoicesNav").addEventListener("click", (event) => {
        event.preventDefault();
        openModal("invoiceModal");
    });
    document.getElementById("chatNav").addEventListener("click", (event) => {
        event.preventDefault();
        openModal("chatModal");
    });
    document.getElementById("generateQrBtn").addEventListener("click", generateDemoQr);
    document.querySelectorAll("[data-close-modal]").forEach((button) => {
        button.addEventListener("click", closeModal);
    });
    document.getElementById("modalBackdrop").addEventListener("click", (event) => {
        if (event.target.id === "modalBackdrop") closeModal();
    });
    document.getElementById("printInvoiceBtn").addEventListener("click", () => window.print());
    document.getElementById("chatForm").addEventListener("submit", (event) => {
        event.preventDefault();
        const input = document.getElementById("chatInput");
        const text = input.value.trim();
        if (!text) return;
        addChatMessage(text, "sent");
        input.value = "";
        window.setTimeout(() => addChatMessage("Thanks — this demo desk can help with routing, invoices, and payment previews.", "received"), 300);
    });

    await Promise.all([
        loadOperationsSummary().catch((error) => {
            console.error("ResQAI operations summary error:", error);
        }),
        generateRoute().catch(() => {})
    ]);
});
