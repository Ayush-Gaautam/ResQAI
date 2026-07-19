#include "../include/Graph.h"

int main()
{
    Graph graph;

    graph.loadGraph("data/processed/edges.csv");

    graph.printGraphInfo();

    return 0;
}