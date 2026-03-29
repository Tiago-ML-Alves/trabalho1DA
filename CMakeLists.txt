
cmake_minimum_required(VERSION 3.20)

project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(main
        code/src/main.cpp
        code/src/Parser.cpp
        code/src/FlowNetwork.cpp
        code/src/Scheduler.cpp
        code/src/Output.cpp
)