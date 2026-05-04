#include "audio_entropy_macos.h"
#include "sha-256.h"
#include <iostream>
#include <thread>
#include <stdexcept>
#include <chrono>

// ---------- Конструкторы ----------
AudioEntropy::AudioEntropy()
    : WhiteBits(2), durationMs(500), mQueue(nullptr), mRunning(false) {
    setupAudioFormat();
}

AudioEntropy::AudioEntropy(int bits_per_sample, unsigned int duration_ms)
    : WhiteBits(bits_per_sample), durationMs(duration_ms), mQueue(nullptr), mRunning(false) {
    setupAudioFormat();
}

// ---------- Деструктор ----------
AudioEntropy::~AudioEntropy() {
    if (mQueue) {
        AudioQueueStop(mQueue, true);
        AudioQueueDispose(mQueue, true);
        mQueue = nullptr;
    }
}

// ---------- Настройка формата ----------
void AudioEntropy::setupAudioFormat() {
    mFormat.mFormatID         = kAudioFormatLinearPCM;
    mFormat.mFormatFlags      = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    mFormat.mSampleRate       = 44100.0;
    mFormat.mChannelsPerFrame = 1;               // моно
    mFormat.mBitsPerChannel   = 16;              // 16 бит на семпл
    mFormat.mBytesPerFrame    = (mFormat.mBitsPerChannel / 8) * mFormat.mChannelsPerFrame; // 2 байта
    mFormat.mFramesPerPacket  = 1;
    mFormat.mBytesPerPacket   = mFormat.mBytesPerFrame * mFormat.mFramesPerPacket;
}

// ---------- Открытие устройства ----------
bool AudioEntropy::openDevice() {
    OSStatus status = AudioQueueNewInput(
        &mFormat,
        audioInputCallback,
        this,
        NULL,
        NULL,
        0,
        &mQueue
    );
    if (status != noErr) return false;

    const int kNumberBuffers = 3;
    UInt32 bufferByteSize = static_cast<UInt32>(mFormat.mSampleRate * mFormat.mBytesPerFrame * 0.5);

    for (int i = 0; i < kNumberBuffers; ++i) {
        AudioQueueBufferRef buffer;
        status = AudioQueueAllocateBuffer(mQueue, bufferByteSize, &buffer);
        if (status != noErr) return false;
        status = AudioQueueEnqueueBuffer(mQueue, buffer, 0, NULL);
        if (status != noErr) return false;
    }
    mRunning = true;
    return true;
}

// ---------- Статический callback ----------
void AudioEntropy::audioInputCallback(
    void*                                 inUserData,
    AudioQueueRef                         inAQ,
    AudioQueueBufferRef                   inBuffer,
    const AudioTimeStamp*                 inStartTime,
    UInt32                                inNumberPackets,
    const AudioStreamPacketDescription*   inPacketDescs)
{
    AudioEntropy* recorder = static_cast<AudioEntropy*>(inUserData);
    if (inNumberPackets > 0) {
        const int16_t* audioData = static_cast<const int16_t*>(inBuffer->mAudioData);
        size_t numSamples = inBuffer->mAudioDataByteSize / sizeof(int16_t);
        recorder->rawSamples.insert(recorder->rawSamples.end(),
                                     audioData, audioData + numSamples);
    }
    if (recorder->mRunning) {
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}

// ---------- Захват ----------
void AudioEntropy::capture() {
    rawSamples.clear();
    setupAudioFormat();       // уже вызывалось в конструкторе, но не помешает

    if (!openDevice())
        throw std::runtime_error("open device failed");

    OSStatus status = AudioQueueStart(mQueue, NULL);
    if (status != noErr) {
        AudioQueueDispose(mQueue, true);
        mQueue = nullptr;
        throw std::runtime_error("start capture failed");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));

    // Останавливаем запись
    mRunning = false;
    status = AudioQueueStop(mQueue, true);      // правильная остановка
    // можно проверить status, но для упрощения игнорируем

    AudioQueueDispose(mQueue, true);
    mQueue = nullptr;
}

// ---------- Извлечение младших бит ----------
std::vector<uint8_t> AudioEntropy::extractBits(const std::vector<int16_t>& samples) const {
    std::vector<uint8_t> bits;
    bits.reserve(samples.size() * WhiteBits);
    for (int16_t s : samples) {
        for (int b = 0; b < WhiteBits; ++b) {
            bits.push_back((s >> b) & 1);
        }
    }
    return bits;
}

// ---------- Упаковка бит в байты ----------
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

// ---------- Отладочные методы ----------
std::vector<int16_t> AudioEntropy::getRawSamples() const {
    return rawSamples;
}

std::vector<uint8_t> AudioEntropy::getExtractBits() const {
    if (rawSamples.empty())
        throw std::runtime_error("No samples captured");
    return extractBits(rawSamples);
}

std::vector<uint8_t> AudioEntropy::getCleanedBits() const {
    auto bits = getExtractBits();   // выкинет исключение, если rawSamples пуст
    return packBits(bits);
}

// ---------- Финальный хеш ----------
std::vector<uint8_t> AudioEntropy::getRandom() {
    capture();
    auto bits = extractBits(rawSamples);
    auto packed = packBits(bits);
    return SHA256::hash(packed);
}