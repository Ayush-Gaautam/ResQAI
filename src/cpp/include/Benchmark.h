#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "Graph.h"
#include "Dijkstra.h"
#include "AStar.h"

class Benchmark
{
private:

    const Graph& graph;

public:

    Benchmark(const Graph& g);

    void compareAlgorithms(
        long long source,
        long long destination
    );
};

#endif
