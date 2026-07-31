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
    std::unordered_map<long long, double> distance;

    const auto& adjacencyList = graph.getGraph();

    // Initialize distances
    for (const auto& node : adjacencyList)
    {
        distance[node.first] = std::numeric_limits<double>::infinity();
    }

    distance[source] = 0.0;
    parent[source] = -1;

    using State = std::pair<double, long long>;

    std::priority_queue<
        State,
        std::vector<State>,
        std::greater<State>
    > pq;

    pq.push({0.0, source});

    while (!pq.empty())
    {
        double currentDistance = pq.top().first;
        long long currentNode = pq.top().second;
        pq.pop();

        // Ignore outdated entries
        if (currentDistance > distance[currentNode])
        {
            continue;
        }

        // Skip nodes with no outgoing edges
        if (adjacencyList.find(currentNode) == adjacencyList.end())
        {
            continue;
        }

        // Visit neighbours
        for (const Edge& edge : adjacencyList.at(currentNode))
        {
            if (edge.blocked)
            {
                continue;
            }

            double edgeCost =
                edge.length * edge.trafficMultiplier;

            double newDistance =
                currentDistance + edgeCost;

            // Initialize destination if missing
            if (distance.find(edge.destination) == distance.end())
            {
                distance[edge.destination] =
                    std::numeric_limits<double>::infinity();
            }

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

    while (parent.find(current) != parent.end() &&
           parent.at(current) != -1)
    {
        path.push_back(current);
        current = parent.at(current);
    }

    path.push_back(current);

    std::reverse(path.begin(), path.end());

    return path;
}