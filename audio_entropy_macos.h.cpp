#include "audio_entropy_macos.h"
#include <iostream>
#include <AudioToolbox/AudioToolbox.h>

AudioEntropy::AudioEntropy(): WhiteBits(16), durationMs(500) {}

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

void AudioEntropy::capture() {
    rawSamples.clear();
    setupAudioFormat();

    if (!openDevice()) {
        throw std::runtime_error("open device failed");
    }
    OSStatus status = AudioQueueStart(mQueue, NULL);
    if (status != noErr) {
        AudioQueueDispose(mQueue, NULL);
        throw std::runtime_error("open capture failed");
    }
}
