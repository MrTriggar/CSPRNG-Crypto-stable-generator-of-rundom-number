cmake_minimum_required(VERSION 3.10)

project(CSPRNG-Crypto-stable-generator-of-rundom-number)

set(CMAKE_CXX_STANDARD 11)

add_executable(CSPRNG-Crypto-stable-generator-of-rundom-number "ADC_Generator_main.cpp" "sha-256.h" "sha-256.cpp" "audio_entropy.h" "audio_entropy.cpp" "ErrorTypes.h")

target_link_libraries(CSPRNG-Crypto-stable-generator-of-rundom-number winmm)