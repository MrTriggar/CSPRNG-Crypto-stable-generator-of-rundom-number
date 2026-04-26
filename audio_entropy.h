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
    std::vector<int16_t> rawSamples; // сырые сэмплы по 16 бит
public:
    AudioEntropy();
    AudioEntropy(int bits_per_sample, DWORD duration_ms);

    bool isOpenDevice(UINT deviceId, WAVEFORMATEX& wfx, HWAVEIN& hWaveOut);             // проверка возможности открытия устроства и непосредственно открытие устройства
    void capture();                                                                     // вся работа с получением сырых сэпмлов
    std::vector<uint8_t> extractBits(const std::vector<int16_t>& samples) const;        // из сырых сэмплов извлекает n младших битов и делает 0/1 из них
    std::vector<uint8_t> packBits(const std::vector<uint8_t>& bits) const;              // упаковывает extractBits

    std::vector<int16_t> getRawSamples() const;                                         // функция отладки, получение сырых только что считанных сэмплов (использовать после capture(), иначе rawSamples будет пустым)
    std::vector<uint8_t> getExtractBits() const;                                        // функция отладки, получание n последних бит от сырых сэмплов (использовать после capture(), иначе rawSamples будет пустым)
    std::vector<uint8_t> getCleanedBits() const;                                        // функция отладки, получание сжатых байт: вместо 8 бит ради 1-2 последних бит из сэпмлов укоплектовывает все биты подряд (использовать после capture(), иначе rawSamples будет пустым)
    std::vector<uint8_t> getRandom();                                                   // финальная функция, возвращающает рандомный хэш sha-256
};