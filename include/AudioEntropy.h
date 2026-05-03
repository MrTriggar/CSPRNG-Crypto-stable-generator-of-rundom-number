#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <cstdint>
#include <string>

class AudioEntropy {
private:
    int bitsPerSample;
    DWORD durationMs;
    std::vector<int16_t> rawSamples;

public:
    AudioEntropy();
    AudioEntropy(int bits_per_sample, DWORD duration_ms);

    bool isOpenDevice(UINT deviceId, WAVEFORMATEX& wfx, HWAVEIN& hWaveOut);
    void capture();
    std::vector<uint8_t> extractBits(const std::vector<int16_t>& samples) const;
    std::vector<uint8_t> packBits(const std::vector<uint8_t>& bits) const;

    std::vector<int16_t> getRawSamples() const;
    std::vector<uint8_t> getExtractBits() const;
    std::vector<uint8_t> getCleanedBits() const;
};