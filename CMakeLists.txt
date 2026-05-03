cmake_minimum_required(VERSION 3.10)
project(Prodject)
set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(Prodject
        main.cpp
        src/AudioEntropy.cpp
        src/CPUEntropySource.cpp
        src/SHA256.cpp
        src/EntropyPool.cpp
        srcDUO/DuoPull.cpp
        srcDUO/AdminSystem.cpp
)

target_link_libraries(Prodject winmm)