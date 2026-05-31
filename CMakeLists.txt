cmake_minimum_required(VERSION 4.2)
project(IsingModel)

set(CMAKE_CXX_STANDARD 20)

add_executable(IsingModel main.cpp
        include/IsingLattice.h
        src/IsingLattice.cpp
        include/MonteCarlos.h
        src/MonteCarlos.cpp)
