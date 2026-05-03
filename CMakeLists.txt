cmake_minimum_required(VERSION 3.10)

project(CSPRNG-Crypto-stable-generator-of-rundom-number)

set(CMAKE_CXX_STANDARD 11)

add_executable(Prodject main.cpp
        include/CPUEntropySource.hpp
        src/CPUEntropySource.cpp
        include/SHA256.hpp
        src/SHA256.cpp)