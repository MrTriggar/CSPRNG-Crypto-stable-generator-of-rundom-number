#include <cstring>
#include <sstream>
#include <iostream>
#include "audio_entropy.h"
#include "sha-256.h"
#include "ErrorTypes.h"

#pragma comment(lib, "winmm.lib") // подключение библиотеки winmm.lib для работы с аудиовходами

AudioEntropy::AudioEntropy() : bitsPerSample(2), durationMs(500){}

AudioEntropy::AudioEntropy(int bits_per_sample, DWORD duration_ms) : bitsPerSample(bits_per_sample), durationMs(duration_ms) {}

bool AudioEntropy::isOpenDevice(UINT deviceId, WAVEFORMATEX& wfx, HWAVEIN& hWaveOut) {
    MMRESULT res = waveInOpen(&hWaveOut, deviceId, &wfx, 0, 0, CALLBACK_NULL);
    if (res == MMSYSERR_NOERROR) return true;
    else throw exeption("Device open failed");
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
    if (!isOpenDevice(WAVE_MAPPER, wfx, hWave)) throw exeption("open failed");

    // проверяем размер ожидаемы данных
    DWORD bytesToCapture = (DWORD)((double)wfx.nAvgBytesPerSec * durationMs / 1000.0);
    DWORD samplesToCapture = bytesToCapture / wfx.nBlockAlign;
    bytesToCapture = samplesToCapture * wfx.nBlockAlign;
    if (bytesToCapture == 0)
    {
        waveInClose(hWave);
        throw exeption("Duration too short");
    }

    // выделение буфера (README -> func -> 1)
    LPBYTE buffer = (LPBYTE)GlobalAlloc(GPTR, bytesToCapture);
    if (!buffer)
    {
        waveInClose(hWave);
        throw exeption("GlobalAlloc failed");
    }

    // настройки буфера
    WAVEHDR waveHdr = {};
    waveHdr.lpData = (LPSTR)buffer;
    waveHdr.dwBufferLength = bytesToCapture;
    waveHdr.dwFlags = 0;

    // подготовка буфера
    if (waveInPrepareHeader(hWave, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
    {
        GlobalFree(buffer);
        waveInClose(hWave);
        throw exeption("waveInPrepareHeader failed");
    }

    // доабвление буфера в очередь
    if (waveInAddBuffer(hWave, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
    {
        waveInUnprepareHeader(hWave, &waveHdr, sizeof(WAVEHDR));
        GlobalFree(buffer);
        waveInClose(hWave);
        throw exeption("waveInAddBuffer failed");
    }

    // запуск прослушивания
    waveInStart(hWave);

    // ждём заполнения буфера, проверяем на timeout
    DWORD timeout = durationMs + 500;
    DWORD startTime = GetTickCount64();
    while (!(waveHdr.dwFlags & WHDR_DONE)) {
        if (GetTickCount64() - startTime > timeout)
        {
            waveInStop(hWave);
            throw exeption("Capture timeout");
        }
        Sleep(10);
    }

    // останавливаем запись
    waveInStop(hWave);

    // явно задаем размер для сырых сэмплов
    rawSamples.resize(samplesToCapture);
    // копируем сэмплы из буфера с вектор
    memcpy(rawSamples.data(), buffer, bytesToCapture);

    //првоерка, что memcpy сработал
    if (rawSamples.empty()) throw exeption("memcpy failed, rawSamples is empty");

    // Проверка, что не все нули
    bool allZero = true;
    size_t step = rawSamples.size() / 100;
    for (size_t i = 0; i < rawSamples.size() && i < 100; ++i) {
        size_t index = i * step;
        if (rawSamples[index] != 0) {
            allZero = false;
            break;
        }
    }
    if (allZero) throw exeption("rawSamples is all zero");

    // освобождение ресурсов
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

std::vector<int16_t> AudioEntropy::getRawSamples() const
{
    return rawSamples;
}

std::vector<uint8_t> AudioEntropy::getExtractBits() const
{
    if (rawSamples.empty()) throw exeption("rawSamples is empty");
    std::vector<uint8_t> bits = extractBits(rawSamples);
    return bits;
}

std::vector<uint8_t> AudioEntropy::getCleanedBits() const
{
    if (rawSamples.empty()) throw exeption("rawSamples is empty");
    std::vector<uint8_t> bits = extractBits(rawSamples);
    return packBits(bits);
}

std::vector<uint8_t> AudioEntropy::getRandom()
{
    capture();
    auto bits = extractBits(rawSamples);
    auto cleaned = packBits(bits);
    if (cleaned.empty()) throw exeption("Vector is empty");
    auto hash = SHA256::hash(cleaned);
    return hash;
}