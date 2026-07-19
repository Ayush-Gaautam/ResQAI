import os
import pandas as pd
import osmnx as ox

print("=" * 50)
print("Loading Chandigarh Graph...")
print("=" * 50)

# Load GraphML
G = ox.load_graphml("data/raw/chandigarh.graphml")

print("Graph Loaded Successfully!")

# Convert graph to GeoDataFrames
nodes, edges = ox.graph_to_gdfs(G)

# Create processed folder if needed
os.makedirs("data/processed", exist_ok=True)

# -------------------------------
# Export Nodes
# -------------------------------
print("\nExporting Nodes...")

nodes_df = pd.DataFrame({
    "node_id": nodes.index,
    "longitude": nodes["x"],
    "latitude": nodes["y"]
})

nodes_df.to_csv(
    "data/processed/nodes.csv",
    index=False
)

print("nodes.csv created!")

# -------------------------------
# Export Edges
# -------------------------------
print("\nExporting Edges...")

edges_df = pd.DataFrame({
    "source": edges.index.get_level_values(0),
    "destination": edges.index.get_level_values(1),
    "length": edges["length"]
})

edges_df.to_csv(
    "data/processed/edges.csv",
    index=False
)

print("edges.csv created!")

print("\nGraph Export Completed!")