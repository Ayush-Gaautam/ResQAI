import pandas as pd
import folium

# Load data
nodes = pd.read_csv("data/processed/nodes.csv")
path = pd.read_csv("outputs/path.csv")

# Merge using the correct column names
route = path.merge(nodes, left_on="nodeId", right_on="node_id")

if route.empty:
    print("No route data found after merging!")
    exit()

# Create map centered at the first node
m = folium.Map(
    location=[route.iloc[0]["latitude"], route.iloc[0]["longitude"]],
    zoom_start=16
)

# Route coordinates
coordinates = route[["latitude", "longitude"]].values.tolist()

# Draw route
folium.PolyLine(
    coordinates,
    color="blue",
    weight=6,
    opacity=0.8
).add_to(m)

# Start marker
folium.Marker(
    coordinates[0],
    popup="Source",
    icon=folium.Icon(color="green")
).add_to(m)

# Hospital marker
folium.Marker(
    coordinates[-1],
    popup="Nearest Hospital",
    icon=folium.Icon(color="red")
).add_to(m)

# Save map
m.save("visualization/route.html")

print("Map saved successfully to visualization/route.html")