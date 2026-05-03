#include "EntropyPool.h"
#include "SHA256.h"
#include <algorithm>

EntropyPool::EntropyPool() : mixCounter(0) {
    pool.resize(32, 0);
}

void EntropyPool::addEntropy(const std::vector<uint8_t>& data) {
    if (data.empty()) return;

    std::vector<uint8_t> toHash;
    toHash.reserve(pool.size() + data.size() + sizeof(mixCounter));
    toHash.insert(toHash.end(), pool.begin(), pool.end());
    toHash.insert(toHash.end(), data.begin(), data.end());
    for (size_t i = 0; i < sizeof(mixCounter); ++i)
        toHash.push_back((mixCounter >> (i * 8)) & 0xFF);
    mixCounter++;

    pool = SHA256::hash(toHash);
}

std::vector<uint8_t> EntropyPool::getRandomBytes(size_t count) {
    std::vector<uint8_t> result;
    result.reserve(count);

    while (result.size() < count) {
        auto hash = SHA256::hash(pool);
        size_t toCopy = std::min(hash.size(), count - result.size());
        result.insert(result.end(), hash.begin(), hash.begin() + toCopy);
        addEntropy(hash);
    }
    return result;
}

void EntropyPool::reset() {
    pool.assign(32, 0);
    mixCounter = 0;
}