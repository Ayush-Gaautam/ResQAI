#include "../include/Graph.h"
#include "../include/Dijkstra.h"
#include "../include/AStar.h"
#include "../include/Benchmark.h"
#include "../include/Traffic.h"
#include "../include/Ambulance.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <fstream>

int main()
{
    // =====================================
    // Load Graph and Hospitals
    // =====================================
    Graph graph;
    
    

    graph.loadNodes("data/processed/nodes.csv");
    graph.loadGraph("data/processed/edges.csv");
    graph.loadHospitals("data/processed/hospitals.csv");
    Traffic traffic;

    traffic.loadTraffic(
        graph,
        "data/traffic.csv"
    );
    Ambulance ambulance;

    ambulance.loadAmbulances(
        "data/processed/ambulances.csv"
    );

    graph.printGraphInfo();

    // =====================================
    // Create Dijkstra Object
    // =====================================
    Dijkstra dijkstra(graph);
    AStar astar(graph);

    // Parent Table
    std::unordered_map<long long, long long> parent;

    // Source Node
    // Emergency Node
    long long source;

    std::cout << "\nEnter Emergency Node ID: ";
    std::cin >> source;

    AmbulanceInfo nearestAmbulance =
    ambulance.findNearestAvailableAmbulance(
        graph,
        source
    );
    if (nearestAmbulance.nodeId == -1)
    {
        std::cout << "No available ambulance found.\n";
        return 0;
    }

// Continue normally
    std::cout << "Nearest Ambulance: "
          << nearestAmbulance.ambulanceId
          << std::endl;

    std::cout << "\n========== NEAREST AMBULANCE ==========\n";
    std::cout << "Ambulance ID : "
          << nearestAmbulance.ambulanceId
          << std::endl;

    std::cout << "Node ID      : "
          << nearestAmbulance.nodeId
          << std::endl;

    std::cout << "Status       : "
          << nearestAmbulance.status
          << std::endl;

    // =====================================
    // Ambulance -> Patient Route
    // =====================================

    std::unordered_map<long long, long long> ambulanceParent;

    
    astar.shortestPath(
        nearestAmbulance.nodeId,
        source,
        ambulanceParent
    );

    auto ambulancePath =
        astar.reconstructPath(
            source,
            ambulanceParent
        );

    // Run Dijkstra
    auto distance = dijkstra.shortestPath(source, parent);

    // =====================================
    // Find Nearest Hospital
    // =====================================
    double bestDistance = std::numeric_limits<double>::infinity();

    Hospital nearestHospital;
    bool found = false;

    for (const Hospital& hospital : graph.getHospitals())
    {
        auto it = distance.find(hospital.nodeId);

        if (it == distance.end())
            continue;

        if (it->second < bestDistance)
        {
            bestDistance = it->second;
            nearestHospital = hospital;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "\nNo reachable hospital found." << std::endl;
        return 0;
    }
    

    // =====================================
    // Reconstruct Path
    // =====================================
    auto patientHospitalPath =
    dijkstra.reconstructPath(
        nearestHospital.nodeId,
        parent
    );

    // =====================================
    // Print Results
    // =====================================
    std::cout << "\n=========================================\n";
    std::cout << "       NEAREST HOSPITAL FOUND\n";
    std::cout << "=========================================\n";

    std::cout << "Hospital Name : "
              << nearestHospital.name
              << std::endl;

    std::cout << "Hospital Node : "
              << nearestHospital.nodeId
              << std::endl;

    std::cout << "Shortest Distance : "
              << bestDistance
              << " meters"
              << std::endl;

    std::cout << "\nShortest Path\n";
    std::cout << "-----------------------------------------\n";


    for(size_t i=0;i<patientHospitalPath.size();i++)
    {
        std::cout << patientHospitalPath[i];

        if (i != patientHospitalPath.size() - 1)
            std::cout << " -> ";
    }

    std::cout << std::endl;

    // =====================================
    // Save Routes to CSV
    // =====================================
// =====================================
// Save Ambulance -> Patient Route
// =====================================

    std::ofstream ambulanceFile(
        "outputs/ambulance_to_patient.csv"
    );

    ambulanceFile << "nodeId,latitude,longitude\n";

    for(long long nodeId : ambulancePath)
    {
        const Node& node = graph.getNode(nodeId);

        ambulanceFile
            << node.id << ","
            << node.latitude << ","
            << node.longitude << "\n";
    }

    ambulanceFile.close();
    if(!ambulanceFile)
    {
        std::cout << "Error writing ambulance route.\n";
    }
    // =====================================
// Save Patient -> Hospital Route
// =====================================

    std::ofstream hospitalFile(
        "outputs/patient_to_hospital.csv"
    );

    hospitalFile << "nodeId,latitude,longitude\n";

    for(long long nodeId : patientHospitalPath)
    {
        const Node& node = graph.getNode(nodeId);

        hospitalFile
            << node.id << ","
            << node.latitude << ","
            << node.longitude << "\n";
    }

    hospitalFile.close();
    if(!hospitalFile)
    {
        std::cout << "Error writing hospital route.\n";
    }

    std::cout << "\nAmbulance route saved to outputs/ambulance_to_patient.csv\n";
    std::cout << "\nHospital route saved to outputs/patient_to_hospital.csv\n";
    // ====================================
    // Benchmark
    // ====================================

    Benchmark benchmark(graph);

    benchmark.compareAlgorithms(
        source,
        nearestHospital.nodeId
    );

    return 0;
}
