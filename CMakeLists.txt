
cmake_minimum_required(VERSION 3.20)

project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(main
        Project1SampleDatasets/src/main.cpp
        Project1SampleDatasets/src/Parser.cpp
        Project1SampleDatasets/src/FlowNetwork.cpp
        Project1SampleDatasets/src/Scheduler.cpp
        Project1SampleDatasets/src/Output.cpp
)