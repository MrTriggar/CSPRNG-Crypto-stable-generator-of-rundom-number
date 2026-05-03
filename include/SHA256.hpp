#pragma once
#include <vector>
#include <cstdint>

class SHA256 {
public:
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data);
};