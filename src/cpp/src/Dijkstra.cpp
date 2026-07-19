#include "../include/Dijkstra.h"
#include <queue>
#include <limits>
#include <functional>
#include <algorithm>

// Constructor
Dijkstra::Dijkstra(const Graph& g)
    : graph(g)
{
}

// Dijkstra Algorithm
std::unordered_map<long long, double>
Dijkstra::shortestPath(
    long long source,
    std::unordered_map<long long, long long>& parent
)
{
    // Distance table
    std::unordered_map<long long, double> distance;

    // Get the graph
    const auto& adjacencyList = graph.getGraph();

    // Initialize every node with infinity
    for (const auto& node : adjacencyList)
    {
        distance[node.first] = std::numeric_limits<double>::infinity();
    }

    // Source node distance is 0
    distance[source] = 0.0;

    // Source has no parent
    parent[source] = -1;

    // Min Heap
    std::priority_queue<
        std::pair<double, long long>,
        std::vector<std::pair<double, long long>>,
        std::greater<std::pair<double, long long>>
    > pq;

    // Push source node
    pq.push({0.0, source});

    while (!pq.empty())
    {
        // Current node
        double currentDistance = pq.top().first;
        long long currentNode = pq.top().second;

        pq.pop();

        // Visit neighbours
        for (const Edge& edge : adjacencyList.at(currentNode))
        {
            double newDistance = currentDistance + edge.length;

            // Better path found
            if (newDistance < distance[edge.destination])
            {
                distance[edge.destination] = newDistance;

                parent[edge.destination] = currentNode;

                pq.push({newDistance, edge.destination});
            }
        }
    }

    return distance;
}


// Reconstruct Shortest Path
std::vector<long long>
Dijkstra::reconstructPath(
    long long destination,
    const std::unordered_map<long long, long long>& parent
)
{
    std::vector<long long> path;

    long long current = destination;

    while (parent.find(current) != parent.end() && parent.at(current) != -1)
    {
        path.push_back(current);

        current = parent.at(current);
    }
    // Add the source node
    path.push_back(current);

    // Reverse the path
    std::reverse(path.begin(), path.end());

    return path;
}