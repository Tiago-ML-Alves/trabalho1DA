
cmake_minimum_required(VERSION 3.20)
project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(main Project1SampleDatasets/src/main.cpp
)
add_executable(parser Project1SampleDatasets/src/Parser.cpp)

add_executable(review_tool main.cpp)
