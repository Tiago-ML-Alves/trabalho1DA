cmake_minimum_required(VERSION 4.1)
project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(main Project1SampleDatasets/src/main.cpp
        Project1SampleDatasets/src/Parser.h)
add_executable(parser
        Project1SampleDatasets/src/Parser.cpp)
add_executable(build_flow
        Project1SampleDatasets/src/FlowNetwork.cpp)
add_executable(scheduler
        Project1SampleDatasets/src/Scheduler.cpp)
add_executable(output
        Project1SampleDatasets/src/Output.cpp)
