import os
import osmnx as ox

place = "Chandigarh, India"

print("Downloading road network...")

G = ox.graph_from_place(place, network_type="drive")

# Create the folder if it doesn't exist
os.makedirs("data/raw", exist_ok=True)

# Save inside your project
ox.save_graphml(G, "data/raw/chandigarh.graphml")

print("Download completed!")
print("Saved to: data/raw/chandigarh.graphml")