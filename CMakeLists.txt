cmake_minimum_required(VERSION 4.1)
project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(main Project1SampleDatasets/src/main.cpp
        Project1SampleDatasets/src/Parser.h)
add_executable(parser
        Project1SampleDatasets/src/Parser.cpp)

add_executable(review_tool main.cpp)