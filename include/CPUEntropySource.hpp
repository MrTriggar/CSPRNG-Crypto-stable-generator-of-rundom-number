#pragma once
#include <cstdint>
#include <vector>

class CPUEntropySource {
public:
    CPUEntropySource();

    void                    collectRawData(size_t);
    std::vector<uint8_t>    getRawBytes()   const;
    std::vector<uint8_t>    getRandom();
private:
    uint64_t                previousDeltaValue;
    bool                    hasPreviousDelta;
    std::vector<uint8_t>    rawBytes;

    void                    workload(uint8_t*, size_t);
    uint8_t                 reapDelta(uint64_t);
};