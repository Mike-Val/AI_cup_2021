cmake_minimum_required(VERSION 3.21)
project(AI_cup_2021)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

add_executable(AI_cup_2021
        TwoOPT.hpp
        main.cpp
        Problem.hpp
        AntColony.hpp)
