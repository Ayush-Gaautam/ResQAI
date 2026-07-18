import heapq


def dijkstra(graph, start):
    """
    Finds the shortest distance from the start node
    to every other node in the graph.
    """

    # Distance table
    distance = {}

    # Parent table
    parent = {}

    # Initialize every node
    for node in graph.nodes:
        distance[node] = float("inf")
        parent[node] = None

    # Starting point
    distance[start] = 0

    # Priority Queue
    priority_queue = []

    heapq.heappush(priority_queue, (0, start))

    while priority_queue:

        current_distance, current_node = heapq.heappop(priority_queue)

        # Ignore outdated entries
        if current_distance > distance[current_node]:
            continue

        # Explore neighbours
        for neighbour in graph.neighbors(current_node):

            edge_data = graph[current_node][neighbour]

            # Road length in meters
            weight = edge_data[0].get("length", 1)

            new_distance = current_distance + weight

            if new_distance < distance[neighbour]:

                distance[neighbour] = new_distance

                parent[neighbour] = current_node

                heapq.heappush(
                    priority_queue,
                    (new_distance, neighbour)
                )

    return distance, parent