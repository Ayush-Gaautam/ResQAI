#include "../include/Graph.h"
#include "../include/Dijkstra.h"
#include "../include/AStar.h"
#include "../include/Benchmark.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <fstream>

int main()
{
    // =====================================
    // Load Graph and Hospitals
    // =====================================
    Graph graph;
    
    

    graph.loadNodes("data/processed/nodes.csv");
    graph.loadGraph("data/processed/edges.csv");
    graph.loadHospitals("data/processed/hospitals.csv");

    graph.printGraphInfo();

    // =====================================
    // Create Dijkstra Object
    // =====================================
    Dijkstra dijkstra(graph);

    // Parent Table
    std::unordered_map<long long, long long> parent;

    // Source Node
    long long source = 813348627;

    // Run Dijkstra
    auto distance = dijkstra.shortestPath(source, parent);

    // =====================================
    // Find Nearest Hospital
    // =====================================
    double bestDistance = std::numeric_limits<double>::infinity();

    Hospital nearestHospital;
    bool found = false;

    for (const Hospital& hospital : graph.getHospitals())
    {
        auto it = distance.find(hospital.nodeId);

        if (it == distance.end())
            continue;

        if (it->second < bestDistance)
        {
            bestDistance = it->second;
            nearestHospital = hospital;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "\nNo reachable hospital found." << std::endl;
        return 0;
    }
    

    // =====================================
    // Reconstruct Path
    // =====================================
    auto path = dijkstra.reconstructPath(nearestHospital.nodeId, parent);

    // =====================================
    // Print Results
    // =====================================
    std::cout << "\n=========================================\n";
    std::cout << "       NEAREST HOSPITAL FOUND\n";
    std::cout << "=========================================\n";

    std::cout << "Hospital Name : "
              << nearestHospital.name
              << std::endl;

    std::cout << "Hospital Node : "
              << nearestHospital.nodeId
              << std::endl;

    std::cout << "Shortest Distance : "
              << bestDistance
              << " meters"
              << std::endl;

    std::cout << "\nShortest Path\n";
    std::cout << "-----------------------------------------\n";


    for (size_t i = 0; i < path.size(); i++)
    {
        std::cout << path[i];

        if (i != path.size() - 1)
            std::cout << " -> ";
    }

    std::cout << std::endl;

    // =====================================
    // Save Path to CSV
    // =====================================
    std::ofstream outFile("outputs/path.csv");

    if (!outFile.is_open())
    {
        std::cout << "\nError creating outputs/path.csv\n";
        return 1;
    }

    outFile << "nodeId\n";

    for (long long node : path)
    {
        outFile << node << "\n";
    }

    outFile.close();

    std::cout << "\nPath saved successfully to outputs/path.csv\n";
    // ====================================
// Benchmark
// ====================================

    Benchmark benchmark(graph);

    benchmark.compareAlgorithms(
        source,
        nearestHospital.nodeId
    );

    return 0;
}
