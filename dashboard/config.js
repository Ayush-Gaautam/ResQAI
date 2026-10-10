// Use the same-origin Vercel API service in production and the local API in development.
//const isLocalDashboard = ["localhost", "127.0.0.1"].includes(window.location.hostname);
//window.RESQAI_API_URL = window.RESQAI_API_URL ||
 //   (isLocalDashboard ? "http://127.0.0.1:8000" : "/api");



const isLocalDashboard = ["localhost", "127.0.0.1"].includes(
    window.location.hostname
);

window.RESQAI_API_URL =
    window.RESQAI_API_URL ||
    (isLocalDashboard
        ? "http://127.0.0.1:8000"
        : "https://resqai-86rk.onrender.com");
