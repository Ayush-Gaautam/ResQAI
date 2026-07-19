#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>

// ===========================
// Road Node
// ===========================


struct Node
{
    long long id;
    double latitude;
    double longitude;
};

// ===========================
// Road Edge
// ===========================
struct Edge
{
    long long destination;
    double length;

    // Future Traffic Simulation
    bool blocked = false;
    double trafficMultiplier = 1.0;
};

// ===========================
// Hospital
// ===========================
struct Hospital
{
    long long nodeId;
    std::string name;
};

// ===========================
// Graph Class
// ===========================
class Graph
{
private:

    // Node Information
    std::unordered_map<long long, Node> nodes;

    // Adjacency List
    std::unordered_map<
        long long,
        std::vector<Edge>
    > adjacencyList;

    // Hospital List
    std::vector<Hospital> hospitals;

public:

    // ===========================
    // Load Data
    // ===========================

    // Load nodes.csv
    const Node& getNode(long long id) const;
    void loadNodes(const std::string& filename);

    // Load edges.csv
    void loadGraph(const std::string& filename);

    // Load hospitals.csv
    void loadHospitals(const std::string& filename);

    // ===========================
    // Display
    // ===========================

    void printGraphInfo() const;

    // ===========================
    // Getters
    // ===========================

    // Return all nodes
    const std::unordered_map<long long, Node>& getNodes() const;

    // Return graph
    const std::unordered_map<long long, std::vector<Edge>>& getGraph() const;

    // Return hospitals
    const std::vector<Hospital>& getHospitals() const;

};

#endif