#ifndef TRAFFIC_H
#define TRAFFIC_H

#include "Graph.h"
#include <string>

class Traffic
{
public:

    // Load traffic information from CSV
    void loadTraffic(
        Graph& graph,
        const std::string& filename
    );
};

#endif