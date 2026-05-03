#include "../include/CPUEntropySource.hpp"
#include "intrin.h"
#include "../include/SHA256.hpp"

CPUEntropySource::CPUEntropySource(): previousDeltaValue(0), hasPreviousDelta(false) {}

void CPUEntropySource::collectRawData(size_t countOfSamples) {
    rawBytes.clear();
    std::vector<uint8_t> buffer(256*1024, 0);

    for (size_t i = 0; i < countOfSamples; i++) {
        uint64_t start = __rdtsc();
        workload(buffer.data(), buffer.size());
        uint64_t end = __rdtsc();

        uint64_t delta = end - start;
        if (hasPreviousDelta) {
            uint64_t deltaRes = delta - previousDeltaValue;
            if (deltaRes != 0) {
                rawBytes.push_back(reapDelta(deltaRes));
            }
        }
        previousDeltaValue = delta;
        hasPreviousDelta = true;
    }
}

std::vector<uint8_t> CPUEntropySource::getRawBytes() const {
    return rawBytes;
}

std::vector<uint8_t> CPUEntropySource::getRandom() {
    auto cleaned = rawBytes;
    auto resultHash = SHA256::hash(cleaned);
    return resultHash;
}

void CPUEntropySource::workload(uint8_t *buffer, size_t size) {
    volatile uint8_t sink = 0;

    for (size_t i = 0; i < size; i++) {
        buffer[i] ^= static_cast<uint8_t>(i);
        sink ^= buffer[i];
    }
}

uint8_t CPUEntropySource::reapDelta(uint64_t delta) {
    uint8_t result = 0;
    result ^= static_cast<uint8_t>(delta);
    result ^= static_cast<uint8_t>(delta >> 8);
    result ^= static_cast<uint8_t>(delta >> 16);
    result ^= static_cast<uint8_t>(delta >> 24);
    result ^= static_cast<uint8_t>(delta >> 32);
    result ^= static_cast<uint8_t>(delta >> 40);
    result ^= static_cast<uint8_t>(delta >> 48);
    result ^= static_cast<uint8_t>(delta >> 56);
    return result;
}