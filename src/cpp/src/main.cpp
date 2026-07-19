#include "../include/Graph.h"
#include "../include/Dijkstra.h"

#include <iostream>
#include <unordered_map>
#include <vector>

int main()
{
    // Load the graph
    Graph graph;

    graph.loadGraph("data/processed/edges.csv");

    graph.printGraphInfo();

    // Create Dijkstra object
    Dijkstra dijkstra(graph);

    // Parent table
    std::unordered_map<long long, long long> parent;

    // Source node
    long long source = 813348627;

    // Run Dijkstra
    auto distance = dijkstra.shortestPath(source, parent);

    // Destination node
    long long destination = 3909471537;

    std::cout << "\nShortest Distance\n";

    std::cout << distance[destination] << " meters\n";

    // Build the shortest path
    auto path = dijkstra.reconstructPath(destination, parent);

    std::cout << "\nShortest Path\n\n";

    for (long long node : path)
    {
        std::cout << node;

        if (node != destination)
            std::cout << " -> ";
    }

    std::cout << std::endl;

    return 0;
}