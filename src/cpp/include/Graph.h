#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>

// Represents one road
struct Edge
{
    long long destination;
    double length;
};

// Graph Class
class Graph
{
private:

    // Adjacency List
    std::unordered_map<
        long long,
        std::vector<Edge>
    > adjacencyList;

public:

    // Read edges.csv
    void loadGraph(const std::string& filename);

    // Display graph information
    void printGraphInfo() const;

    // Return adjacency list
    const std::unordered_map<long long, std::vector<Edge>>& getGraph() const;
};

#endif