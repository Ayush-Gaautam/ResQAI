#include "../include/Traffic.h"

#include <fstream>
#include <sstream>
#include <iostream>

// ===========================
// Load Traffic Information
// ===========================
void Traffic::loadTraffic(
    Graph& graph,
    const std::string& filename
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open traffic file: "
                  << filename << std::endl;
        return;
    }

    auto& adjacencyList = graph.getGraphMutable();

    std::string line;

    // Skip Header
    std::getline(file, line);

    int updatedRoads = 0;
    int blockedRoads = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        std::string source;
        std::string destination;
        std::string multiplier;
        std::string status;

        std::getline(ss, source, ',');
        std::getline(ss, destination, ',');
        std::getline(ss, multiplier, ',');
        std::getline(ss, status);

        try
        {
            long long src = std::stoll(source);
            long long dest = std::stoll(destination);

            double traffic = std::stod(multiplier);

            if (adjacencyList.find(src) == adjacencyList.end())
                continue;

            for (Edge& edge : adjacencyList[src])
            {
                if (edge.destination == dest)
                {
                    edge.trafficMultiplier = traffic;

                    if (traffic == 0.0)
                    {
                        edge.blocked = true;
                        blockedRoads++;
                    }
                    else
                    {
                        edge.blocked = false;
                    }

                    updatedRoads++;
                    break;
                }
            }
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    std::cout << "\n========== TRAFFIC ==========\n";
    std::cout << "Updated Roads : "
              << updatedRoads
              << std::endl;

    std::cout << "Blocked Roads : "
              << blockedRoads
              << std::endl;

    std::cout << "=============================\n";
}