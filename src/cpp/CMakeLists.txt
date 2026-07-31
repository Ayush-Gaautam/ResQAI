cmake_minimum_required(VERSION 3.10)

project(ResQAI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(ResQAI
    src/main.cpp
    src/Graph.cpp
    src/Dijkstra.cpp
    src/AStar.cpp
    src/Traffic.cpp
    src/Ambulance.cpp
    src/Benchmark.cpp
)