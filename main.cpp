#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <intrin.h>
#include "include/CPUEntropySource.hpp"

int main() {
    CPUEntropySource cpu;
    cpu.collectRawData(1024);
    auto result = cpu.getRandom();
    for (uint8_t byte : result) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(byte);
    }
    std::cout << '\n';
}