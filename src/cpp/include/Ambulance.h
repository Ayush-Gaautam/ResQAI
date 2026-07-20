#ifndef AMBULANCE_H
#define AMBULANCE_H

#include "Graph.h"
#include "AStar.h"

#include <vector>
#include <string>

// ===========================
// Ambulance Structure
// ===========================
struct AmbulanceInfo
{
    std::string ambulanceId;
    long long nodeId;
    std::string status;
};

// ===========================
// Ambulance Class
// ===========================
class Ambulance
{
private:

    std::vector<AmbulanceInfo> ambulances;

public:

    // Load ambulances from CSV
    void loadAmbulances(
        const std::string& filename
    );

    // Print all ambulances
    void printAmbulances() const;

    // Find nearest available ambulance
    AmbulanceInfo findNearestAvailableAmbulance(
        const Graph& graph,
        long long emergencyNode
    );

    // Mark ambulance as busy
    void dispatchAmbulance(
        const std::string& ambulanceId
    );

    // Mark ambulance as available
    void releaseAmbulance(
        const std::string& ambulanceId
    );

    // Get all ambulances
    const std::vector<AmbulanceInfo>&
    getAmbulances() const;
};

#endif