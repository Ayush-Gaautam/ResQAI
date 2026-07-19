#ifndef ASTAR_H
#define ASTAR_H

#include "Graph.h"

#include <unordered_map>
#include <vector>

class AStar
{
private:

    const Graph& graph;

    // Haversine heuristic
    double heuristic(long long current,
                     long long destination);

public:

    AStar(const Graph& g);

    std::unordered_map<long long,double> shortestPath(
        long long source,
        long long destination,
        std::unordered_map<long long,long long>& parent
    );

    std::vector<long long> reconstructPath(
        long long destination,
        const std::unordered_map<long long,long long>& parent
    );
};

#endif