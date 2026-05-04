#pragma once
#include <AudioToolbox/AudioToolbox.h>   // нужно для AudioQueue
#include <vector>
#include <cstdint>

class AudioEntropy {
private:
    // Параметры
    int WhiteBits;
    unsigned int durationMs;
    std::vector<int16_t> rawSamples;   // знаковое!

    // Core Audio 
    AudioQueueRef mQueue;
    AudioStreamBasicDescription mFormat;
    bool mRunning = false;

    // Настройка и управление
    void setupAudioFormat();
    bool openDevice();

    void capture();
    void stopAndCleanup();

    // Статический callback для AudioQueue
    static void audioInputCallback(
        void* inUserData,
        AudioQueueRef inAQ,
        AudioQueueBufferRef inBuffer,
        const AudioTimeStamp* inStartTime,
        UInt32 inNumberPackets,
        const AudioStreamPacketDescription* inPacketDescs);

    // Методы обработки бит
    std::vector<uint8_t> extractBits(const std::vector<int16_t>& samples) const;
    std::vector<uint8_t> packBits(const std::vector<uint8_t>& bits) const;
    std::vector<uint8_t> sha256(const std::vector<uint8_t>& data) const; // или использовать SHA256::hash напрямую

public:
    AudioEntropy();
    AudioEntropy(int bitsPerSample, unsigned int durationMs);
    ~AudioEntropy();

    // Публичные функции отладки
    std::vector<int16_t> getRawSamples() const;
    std::vector<uint8_t> getExtractBits() const;
    std::vector<uint8_t> getCleanedBits() const;

    // Основная функция – возвращает хеш SHA‑256 (32 байта)
    std::vector<uint8_t> getRandom();
};
