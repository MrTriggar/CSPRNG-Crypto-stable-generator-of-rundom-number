cmake_minimum_required(VERSION 3.10)
project(CSPRNG-Crypto-stable-generator-of-rundom-number)
set(CMAKE_CXX_STANDARD 17)

add_executable(CSPRNG-Crypto-stable-generator-of-rundom-number
        ADC_Generator_main.cpp
        sha-256.cpp
        audio_entropy_macos.cpp
)

# Эта секция даёт линкеру доступ к CoreAudio/AudioToolbox
target_link_libraries(CSPRNG-Crypto-stable-generator-of-rundom-number
        "-framework AudioToolbox"
        "-framework CoreFoundation"
        "-framework CoreAudio"
)