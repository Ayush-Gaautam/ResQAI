import pandas as pd
import re
from scipy.spatial import KDTree

# -----------------------------
# Load road nodes
# -----------------------------
print("Loading road nodes...")

nodes = pd.read_csv("data/processed/nodes.csv")

# Create list of (latitude, longitude)
road_points = list(zip(nodes["latitude"], nodes["longitude"]))

# Build KDTree
tree = KDTree(road_points)

print("Loaded", len(nodes), "road nodes.")

# -----------------------------
# Load infrastructure
# -----------------------------
print("Loading infrastructure...")

infra = pd.read_csv("data/processed/infrastructure.csv")

# Keep only hospitals
hospitals = infra[infra["amenity"] == "hospital"]

print("Found", len(hospitals), "hospitals.")

hospital_nodes = []

# -----------------------------
# Process each hospital
# -----------------------------
for _, row in hospitals.iterrows():

    geometry = row["geometry"]

    # Extract longitude and latitude from:
    # POINT (76.7762212 30.7611013)
    match = re.search(r'POINT\s*\(([-0-9.]+)\s+([-0-9.]+)\)', geometry)

    if match is None:
        continue

    longitude = float(match.group(1))
    latitude = float(match.group(2))

    # Find nearest road node
    distance, index = tree.query((latitude, longitude))

    nearest_node = int(nodes.iloc[index]["node_id"])

    # Hospital name
    if "name" in hospitals.columns:
        name = row["name"]
    else:
        name = ""

    if pd.isna(name):
        name = "Unknown Hospital"

    hospital_nodes.append({
        "nodeId": nearest_node,
        "name": name
    })

# -----------------------------
# Save CSV
# -----------------------------
output = pd.DataFrame(hospital_nodes)

output.to_csv(
    "data/processed/hospitals.csv",
    index=False
)

print("----------------------------------")
print("Finished Successfully!")
print("Created data/processed/hospitals.csv")
print("Total Hospitals:", len(output))