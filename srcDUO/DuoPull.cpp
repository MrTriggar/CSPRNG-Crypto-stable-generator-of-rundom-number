#include "../includeDUO/DuoPull.h"
#include "SHA256.h"
#include <sstream>
#include <iomanip>

DuoPull::DuoPull() : pullSize(PullSize::BITS_256), mixMethod(MixMethod::RAW_THEN_HASH) {}

DuoPull::DuoPull(PullSize size, MixMethod method) : pullSize(size), mixMethod(method) {}

void DuoPull::setPullSize(PullSize size)      { pullSize = size; }
void DuoPull::setMixMethod(MixMethod method)  { mixMethod = method; }

PullSize  DuoPull::getPullSize()  const { return pullSize; }
MixMethod DuoPull::getMixMethod() const { return mixMethod; }

std::vector<uint8_t> DuoPull::mix(
    const std::vector<uint8_t>& rawMic,
    const std::vector<uint8_t>& rawCpu)
{
    size_t pullBytes = static_cast<size_t>(pullSize);

    auto resizeToPull = [pullBytes](const std::vector<uint8_t>& data) -> std::vector<uint8_t> {
        std::vector<uint8_t> result = data;
        if (result.size() > pullBytes)
            result.resize(pullBytes);
        else if (result.size() < pullBytes)
            result.resize(pullBytes, 0);
        return result;
    };

    std::vector<uint8_t> sizedMic = resizeToPull(rawMic);
    std::vector<uint8_t> sizedCpu = resizeToPull(rawCpu);

    if (mixMethod == MixMethod::RAW_THEN_HASH) {
        std::vector<uint8_t> combined;
        combined.reserve(sizedMic.size() + sizedCpu.size());
        combined.insert(combined.end(), sizedMic.begin(), sizedMic.end());
        combined.insert(combined.end(), sizedCpu.begin(), sizedCpu.end());
        return SHA256::hash(combined);
    }
    else {
        auto hashMic = SHA256::hash(sizedMic);
        auto hashCpu = SHA256::hash(sizedCpu);
        for (size_t i = 0; i < hashMic.size(); ++i)
            hashMic[i] ^= hashCpu[i];
        return hashMic;
    }
}

std::string DuoPull::toHexString(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    for (uint8_t b : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    return oss.str();
}