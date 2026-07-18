import osmnx as ox
import matplotlib.pyplot as plt

# Load the graph
G = ox.load_graphml("data/raw/chandigarh.graphml")

# Plot the road network
fig, ax = ox.plot_graph(
    G,
    node_size=0,
    edge_linewidth=0.5,
    bgcolor="white",
    show=False,
    close=False,
)

plt.title("Chandigarh Road Network")
plt.show()
