#include "../include/Ambulance.h"

#include <fstream>
#include <sstream>
#include <iostream>

// ======================================
// Load Ambulances
// ======================================
void Ambulance::loadAmbulances(
    const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open ambulance file: "
                  << filename << std::endl;
        return;
    }

    ambulances.clear();

    std::string line;

    // Skip Header
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        AmbulanceInfo ambulance;

        std::getline(ss, ambulance.ambulanceId, ',');

        std::string node;

        std::getline(ss, node, ',');

        try
        {
            ambulance.nodeId = std::stoll(node);
        }
        catch(...)
        {
            continue;
        }

        std::getline(ss, ambulance.status);

        ambulances.push_back(ambulance);
    }

    file.close();

    std::cout << "\n========== AMBULANCES ==========\n";
    std::cout << "Loaded : "
              << ambulances.size()
              << std::endl;
    std::cout << "===============================\n";
}

// ======================================
// Print Ambulances
// ======================================
void Ambulance::printAmbulances() const
{
    std::cout << "\n========== AMBULANCE LIST ==========\n";

    for (const auto& ambulance : ambulances)
    {
        std::cout << "ID     : "
                  << ambulance.ambulanceId
                  << std::endl;

        std::cout << "Node   : "
                  << ambulance.nodeId
                  << std::endl;

        std::cout << "Status : "
                  << ambulance.status
                  << std::endl;

        std::cout << "-----------------------------\n";
    }
}
// ======================================
// Find Nearest Available Ambulance
// ======================================
AmbulanceInfo Ambulance::findNearestAvailableAmbulance(
    const Graph& graph,
    long long emergencyNode)
{
    AStar astar(graph);

    double minimumDistance = 1e18;

    AmbulanceInfo nearestAmbulance;

    nearestAmbulance.ambulanceId = "NONE";
    nearestAmbulance.nodeId = -1;
    nearestAmbulance.status = "Unavailable";

    std::unordered_map<long long, long long> parent;

    for (const auto& ambulance : ambulances)
    {
        if (ambulance.status != "Available")
            continue;

        parent.clear();

        auto distance = astar.shortestPath(
            ambulance.nodeId,
            emergencyNode,
            parent);

        if (distance.find(emergencyNode) == distance.end())
            continue;

        if (distance[emergencyNode] < minimumDistance)
        {
            minimumDistance = distance[emergencyNode];
            nearestAmbulance = ambulance;
        }
    }

    return nearestAmbulance;
}
// ======================================
// Dispatch Ambulance
// ======================================
void Ambulance::dispatchAmbulance(
    const std::string& ambulanceId)
{
    for (auto& ambulance : ambulances)
    {
        if (ambulance.ambulanceId == ambulanceId)
        {
            ambulance.status = "Busy";

            std::cout << "\nAmbulance "
                      << ambulanceId
                      << " dispatched successfully.\n";

            return;
        }
    }

    std::cout << "\nAmbulance not found.\n";
}

// ======================================
// Release Ambulance
// ======================================
void Ambulance::releaseAmbulance(
    const std::string& ambulanceId)
{
    for (auto& ambulance : ambulances)
    {
        if (ambulance.ambulanceId == ambulanceId)
        {
            ambulance.status = "Available";

            std::cout << "\nAmbulance "
                      << ambulanceId
                      << " is now available.\n";

            return;
        }
    }

    std::cout << "\nAmbulance not found.\n";
}

// ======================================
// Get Ambulances
// ======================================
const std::vector<AmbulanceInfo>&
Ambulance::getAmbulances() const
{
    return ambulances;
}