#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"

#include <unordered_map>
#include <vector>

class Dijkstra
{
private:
    const Graph& graph;

public:
    // Constructor
    Dijkstra(const Graph& g);

    // Run Dijkstra Algorithm
    std::unordered_map<long long, double> shortestPath(
        long long source,
        std::unordered_map<long long, long long>& parent
    );

    // Reconstruct the shortest path
    std::vector<long long> reconstructPath(
        long long destination,
        const std::unordered_map<long long, long long>& parent
    );
};

#endif