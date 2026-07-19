#include "../include/Benchmark.h"

#include <iostream>
#include <chrono>

Benchmark::Benchmark(const Graph& g)
    : graph(g)
{
}

void Benchmark::compareAlgorithms(
    long long source,
    long long destination
)
{
    Dijkstra dijkstra(graph);
    AStar astar(graph);

    std::unordered_map<long long,long long> parent1;
    std::unordered_map<long long,long long> parent2;

    //-------------------------
    // Dijkstra
    //-------------------------

    auto start1 =
    std::chrono::high_resolution_clock::now();

    auto distance1 =
    dijkstra.shortestPath(
        source,
        parent1
    );

    auto stop1 =
    std::chrono::high_resolution_clock::now();

    //-------------------------
    // A*
    //-------------------------

    auto start2 =
    std::chrono::high_resolution_clock::now();

    auto distance2 =
    astar.shortestPath(
        source,
        destination,
        parent2
    );

    auto stop2 =
    std::chrono::high_resolution_clock::now();

    //-------------------------
    // Time
    //-------------------------

    auto dijkstraTime =
    std::chrono::duration_cast<
    std::chrono::microseconds
    >(stop1-start1);

    auto astarTime =
    std::chrono::duration_cast<
    std::chrono::microseconds
    >(stop2-start2);

    //-------------------------
    // Result
    //-------------------------

    std::cout << "\n==============================\n";
    std::cout << "Benchmark Result\n";
    std::cout << "==============================\n";

    std::cout
    << "Dijkstra Distance : "
    << distance1[destination]
    << " meters\n";

    std::cout
    << "A* Distance       : "
    << distance2[destination]
    << " meters\n";

    std::cout
    << "Dijkstra Time     : "
    << dijkstraTime.count()
    << " microseconds\n";

    std::cout
    << "A* Time           : "
    << astarTime.count()
    << " microseconds\n";

    if(astarTime < dijkstraTime)
        std::cout << "\nWinner : A*\n";
    else
        std::cout << "\nWinner : Dijkstra\n";
}