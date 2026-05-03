#pragma once
#include <vector>
#include <cstdint>

class EntropyPool {
private:
    std::vector<uint8_t> pool;
    size_t mixCounter;

public:
    EntropyPool();

    void addEntropy(const std::vector<uint8_t>& data);
    std::vector<uint8_t> getRandomBytes(size_t count);
    void reset();
};