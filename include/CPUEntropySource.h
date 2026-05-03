#pragma once
#include <cstdint>
#include <vector>

class CPUEntropySource {
public:
    CPUEntropySource();

    void                    collectRawData(size_t countOfSamples);
    std::vector<uint8_t>    getRawBytes() const;

private:
    uint64_t                previousDeltaValue;
    bool                    hasPreviousDelta;
    std::vector<uint8_t>    rawBytes;

    void                    workload(uint8_t* buffer, size_t size);
    uint8_t                 reapDelta(uint64_t delta);
};