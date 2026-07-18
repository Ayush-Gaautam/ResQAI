import osmnx as ox
import pandas as pd
import os

place = "Chandigarh, India"

tags = {
    "amenity": [
        "hospital",
        "police",
        "fire_station"
    ]
}

print("Downloading infrastructure...")

gdf = ox.features_from_place(place, tags)

os.makedirs("data/processed", exist_ok=True)

gdf.to_csv("data/processed/infrastructure.csv")

print("Saved!")
print(gdf[["amenity", "name"]].head())