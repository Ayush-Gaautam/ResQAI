#include "../include/Graph.h"

#include <fstream>
#include <sstream>
#include <iostream>

// ===========================
// Load Nodes (nodes.csv)
// ===========================
void Graph::loadNodes(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open nodes file: "
                  << filename << std::endl;
        return;
    }

    nodes.clear();

    std::string line;

    // Skip Header
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        std::string id;
        std::string longitude;
        std::string latitude;

        std::getline(ss, id, ',');
        std::getline(ss, longitude, ',');
        std::getline(ss, latitude);

        try
        {
            Node node;

            node.id = std::stoll(id);
            node.longitude = std::stod(longitude);
            node.latitude = std::stod(latitude);

            nodes[node.id] = node;
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    std::cout << "Nodes Loaded : "
              << nodes.size()
              << std::endl;
}

// ===========================
// Load Road Graph (edges.csv)
// ===========================
void Graph::loadGraph(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open graph file: "
                  << filename << std::endl;
        return;
    }

    adjacencyList.clear();

    std::string line;

    // Skip Header
    std::getline(file, line);

    long long edgeCount = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        std::string source;
        std::string destination;
        std::string length;

        std::getline(ss, source, ',');
        std::getline(ss, destination, ',');
        std::getline(ss, length, ',');

        try
        {
            long long src = std::stoll(source);
            long long dest = std::stoll(destination);
            double dist = std::stod(length);

            Edge edge;

            edge.destination = dest;
            edge.length = dist;

            // Default traffic values
            edge.blocked = false;
            edge.trafficMultiplier = 1.0;

            adjacencyList[src].push_back(edge);

            edgeCount++;
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    std::cout << "Graph Loaded Successfully\n";
    std::cout << "Edges Loaded : "
              << edgeCount
              << std::endl;
}

// ===========================
// Load Hospitals (hospitals.csv)
// ===========================
void Graph::loadHospitals(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open hospital file: "
                  << filename << std::endl;
        return;
    }

    hospitals.clear();

    std::string line;

    // Skip Header
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        std::string nodeId;
        std::string name;

        std::getline(ss, nodeId, ',');
        std::getline(ss, name);

        try
        {
            Hospital hospital;

            hospital.nodeId = std::stoll(nodeId);
            hospital.name = name;

            hospitals.push_back(hospital);
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    std::cout << "Hospitals Loaded : "
              << hospitals.size()
              << std::endl;
}

// ===========================
// Print Graph Information
// ===========================
void Graph::printGraphInfo() const
{
    long long totalEdges = 0;

    for (const auto& node : adjacencyList)
    {
        totalEdges += node.second.size();
    }

    std::cout << "\n========== GRAPH INFO ==========\n";

    std::cout << "Total Nodes : "
              << nodes.size()
              << std::endl;

    std::cout << "Total Edges : "
              << totalEdges
              << std::endl;

    std::cout << "Hospitals   : "
              << hospitals.size()
              << std::endl;

    std::cout << "===============================\n";
}

// ===========================
// Get Single Node
// ===========================
const Node& Graph::getNode(long long id) const
{
    return nodes.at(id);
}

// ===========================
// Get All Nodes
// ===========================
const std::unordered_map<long long, Node>&
Graph::getNodes() const
{
    return nodes;
}

// ===========================
// Get Graph
// ===========================
const std::unordered_map<long long, std::vector<Edge>>&
Graph::getGraph() const
{
    return adjacencyList;
}

// ===========================
// Get Hospitals
// ===========================
const std::vector<Hospital>&
Graph::getHospitals() const
{
    return hospitals;
}