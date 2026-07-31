#include "../include/AStar.h"

#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>
#include <functional>

constexpr double PI = 3.14159265358979323846;
constexpr double EARTH_RADIUS = 6371000.0;

// Constructor
AStar::AStar(const Graph& g)
    : graph(g)
{
}

// Degree to Radian
static double toRadians(double degree)
{
    return degree * PI / 180.0;
}

// --------------------------------
// Haversine Heuristic
// --------------------------------
double AStar::heuristic(
    long long current,
    long long destination)
{
    const Node& a = graph.getNode(current);
    const Node& b = graph.getNode(destination);

    double lat1 = toRadians(a.latitude);
    double lon1 = toRadians(a.longitude);

    double lat2 = toRadians(b.latitude);
    double lon2 = toRadians(b.longitude);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double x =
        sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1) *
        cos(lat2) *
        sin(dLon / 2) *
        sin(dLon / 2);

    double c =
        2 *
        atan2(
            sqrt(x),
            sqrt(1 - x)
        );

    return EARTH_RADIUS * c;
}
// --------------------------------
// A* Search Algorithm
// --------------------------------
std::unordered_map<long long, double>
AStar::shortestPath(
    long long source,
    long long destination,
    std::unordered_map<long long, long long>& parent
)
{
    // Cost from source
    std::unordered_map<long long, double> gScore;

    // Estimated total cost
    std::unordered_map<long long, double> fScore;

    const auto& adjacencyList = graph.getGraph();

    // Initialize all nodes
    for (const auto& node : adjacencyList)
    {
        gScore[node.first] = std::numeric_limits<double>::infinity();
        fScore[node.first] = std::numeric_limits<double>::infinity();
    }

    gScore[source] = 0.0;
    fScore[source] = heuristic(source, destination);

    parent[source] = -1;

    using State = std::pair<double, long long>;

    std::priority_queue<
        State,
        std::vector<State>,
        std::greater<State>
    > openSet;

    openSet.push({fScore[source], source});

    while (!openSet.empty())
    {
        const State currentState = openSet.top();
        openSet.pop();

        const double currentF = currentState.first;
        const long long current = currentState.second;

        if (currentF > fScore[current])
        {
            continue;
        }

        // Destination reached
        if (current == destination)
            break;
        // Skip if node has no outgoing edges
        if (adjacencyList.find(current) == adjacencyList.end())
            continue;

        // Explore neighbours
        for (const Edge& edge : adjacencyList.at(current))
        {
            // Ignore blocked roads
            if (edge.blocked)
                continue;

            // Apply traffic multiplier
            double edgeCost =
                edge.length * edge.trafficMultiplier;

            double tentativeGScore =
                gScore[current] + edgeCost;

            // Better path found
            if (tentativeGScore < gScore[edge.destination])
            {
                parent[edge.destination] = current;

                gScore[edge.destination] = tentativeGScore;

                fScore[edge.destination] =
                    tentativeGScore +
                    heuristic(edge.destination, destination);
                
                openSet.push(
                {
                    fScore[edge.destination],
                    edge.destination
                });
            }
        }
    }

    return gScore;
}
// --------------------------------
// Reconstruct Shortest Path
// --------------------------------
std::vector<long long>
AStar::reconstructPath(
    long long destination,
    const std::unordered_map<long long, long long>& parent
)
{
    std::vector<long long> path;

    long long current = destination;

    // If destination is unreachable
    if (parent.find(current) == parent.end())
    {
        return path;
    }

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