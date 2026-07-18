from utils.graph_loader import load_graph
from algorithms.dijkstra import dijkstra

graph = load_graph()

# Pick any node from the graph
start_node = list(graph.nodes)[0]

print(f"Starting Node: {start_node}")

distance, parent = dijkstra(graph, start_node)

print("\nDijkstra completed successfully!")
print(f"Total reachable nodes: {len(distance)}")

# Print first 10 distances
count = 0
for node, dist in distance.items():
    print(node, ":", round(dist, 2), "meters")
    count += 1
    if count == 10:
        break
print("\nParent Table")

count = 0
for node, par in parent.items():
    print(node, "<--", par)

    count += 1
    if count == 10:
        break

from utils.path_reconstructor import reconstruct_path

# Pick any destination node for testing
destination = list(graph.nodes)[500]

path = reconstruct_path(parent, destination)

print("\nShortest Path:")

for node in path:
    print(node)