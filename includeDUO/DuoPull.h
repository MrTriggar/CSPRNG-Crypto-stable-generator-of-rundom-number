#pragma once
#include <vector>
#include <cstdint>
#include <string>

enum class PullSize {
    BITS_64   = 8,
    BITS_128  = 16,
    BITS_256  = 32,
    BITS_512  = 64,
    BITS_1024 = 128
};

enum class MixMethod {
    RAW_THEN_HASH,    // смешать сырые, потом хеш
    HASH_THEN_MIX     // хешировать отдельно, потом XOR
};

class DuoPull {
private:
    PullSize  pullSize;
    MixMethod mixMethod;

public:
    DuoPull();
    DuoPull(PullSize size, MixMethod method);

    void setPullSize(PullSize size);
    void setMixMethod(MixMethod method);

    PullSize  getPullSize()  const;
    MixMethod getMixMethod() const;

    std::vector<uint8_t> mix(
        const std::vector<uint8_t>& rawMic,
        const std::vector<uint8_t>& rawCpu
    );

    static std::string toHexString(const std::vector<uint8_t>& data);
};