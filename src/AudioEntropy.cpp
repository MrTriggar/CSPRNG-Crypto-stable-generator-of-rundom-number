#include "AudioEntropy.h"
#include <cstring>
#include <iostream>

#pragma comment(lib, "winmm.lib")

AudioEntropy::AudioEntropy() : bitsPerSample(2), durationMs(500) {}

AudioEntropy::AudioEntropy(int bits_per_sample, DWORD duration_ms)
    : bitsPerSample(bits_per_sample), durationMs(duration_ms) {}

bool AudioEntropy::isOpenDevice(UINT deviceId, WAVEFORMATEX& wfx, HWAVEIN& hWaveOut) {
    MMRESULT res = waveInOpen(&hWaveOut, deviceId, &wfx, 0, 0, CALLBACK_NULL);
    return (res == MMSYSERR_NOERROR);
}

void AudioEntropy::capture() {
    rawSamples.clear();

    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    HWAVEIN hWave = nullptr;
    if (!isOpenDevice(WAVE_MAPPER, wfx, hWave)) return;

    DWORD bytesToCapture = (DWORD)((double)wfx.nAvgBytesPerSec * durationMs / 1000.0);
    DWORD samplesToCapture = bytesToCapture / wfx.nBlockAlign;
    bytesToCapture = samplesToCapture * wfx.nBlockAlign;
    if (bytesToCapture == 0) {
        waveInClose(hWave);
        return;
    }

    LPBYTE buffer = (LPBYTE)GlobalAlloc(GPTR, bytesToCapture);
    if (!buffer) {
        waveInClose(hWave);
        return;
    }

    WAVEHDR waveHdr = {};
    waveHdr.lpData = (LPSTR)buffer;
    waveHdr.dwBufferLength = bytesToCapture;
    waveHdr.dwFlags = 0;

    if (waveInPrepareHeader(hWave, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        GlobalFree(buffer);
        waveInClose(hWave);
        return;
    }

    if (waveInAddBuffer(hWave, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        waveInUnprepareHeader(hWave, &waveHdr, sizeof(WAVEHDR));
        GlobalFree(buffer);
        waveInClose(hWave);
        return;
    }

    waveInStart(hWave);

    DWORD timeout = durationMs + 500;
    DWORD startTime = GetTickCount();
    while (!(waveHdr.dwFlags & WHDR_DONE)) {
        if (GetTickCount() - startTime > timeout) {
            waveInStop(hWave);
            break;
        }
        Sleep(10);
    }

    waveInStop(hWave);

    rawSamples.resize(samplesToCapture);
    memcpy(rawSamples.data(), buffer, bytesToCapture);

    waveInUnprepareHeader(hWave, &waveHdr, sizeof(WAVEHDR));
    GlobalFree(buffer);
    waveInClose(hWave);
}

std::vector<uint8_t> AudioEntropy::extractBits(const std::vector<int16_t>& samples) const {
    std::vector<uint8_t> bits;
    bits.reserve(samples.size() * bitsPerSample);
    for (int16_t s : samples) {
        for (int b = 0; b < bitsPerSample; ++b) {
            bits.push_back((s >> b) & 1);
        }
    }
    return bits;
}

std::vector<uint8_t> AudioEntropy::packBits(const std::vector<uint8_t>& bits) const {
    std::vector<uint8_t> packed;
    size_t byteCount = (bits.size() + 7) / 8;
    packed.resize(byteCount, 0);
    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i])
            packed[i / 8] |= (1 << (i % 8));
    }
    return packed;
}

std::vector<int16_t> AudioEntropy::getRawSamples() const {
    return rawSamples;
}

std::vector<uint8_t> AudioEntropy::getExtractBits() const {
    return extractBits(rawSamples);
}

std::vector<uint8_t> AudioEntropy::getCleanedBits() const {
    return packBits(extractBits(rawSamples));
}