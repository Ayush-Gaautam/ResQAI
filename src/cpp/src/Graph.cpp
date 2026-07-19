#include "../include/Graph.h"

#include <fstream>
#include <sstream>
#include <iostream>

void Graph::loadGraph(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Error opening file!" << std::endl;
        return;
    }

    std::cout << "File opened successfully!" << std::endl;

    std::string line;

    // Skip the header row
    std::getline(file, line);

    // Read the first data row
    while (std::getline(file, line)){
        std::stringstream ss(line);

        std::string source;
        std::string destination;
        std::string length;

        std::getline(ss, source, ',');
        std::getline(ss, destination, ',');
        std::getline(ss, length, ',');

        long long src = std::stoll(source);
        long long dest = std::stoll(destination);
        double dist = std::stod(length);

        Edge edge;

        edge.destination = dest;
        edge.length = dist;

        adjacencyList[src].push_back(edge);

    }

}
const std::unordered_map<long long, std::vector<Edge>>& Graph::getGraph() const
{
    return adjacencyList;
}
void Graph::printGraphInfo() const
{
    std::cout << "Total Nodes : "
              << adjacencyList.size()
              << std::endl;

    long long totalEdges = 0;

    for (const auto& node : adjacencyList)
    {
        totalEdges += node.second.size();
    }

    std::cout << "Total Edges : "
              << totalEdges
              << std::endl;
}
