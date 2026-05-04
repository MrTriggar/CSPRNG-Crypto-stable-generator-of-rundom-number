#include "../includeDUO/AdminSystem.h"
#include "AudioEntropy.h"
#include "CPUEntropySource.h"
#include "SHA256.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <cstdint>

AdminSystem::AdminSystem()
    : mode(SourceMode::BOTH),
      pullSize(PullSize::BITS_256)
{
}

void AdminSystem::setMode(SourceMode m)             { mode = m; }
void AdminSystem::setPullSize(PullSize size)         { pullSize = size; }
void AdminSystem::setDuoConfig(PullSize size, MixMethod method) { duo = DuoPull(size, method); }

SourceMode AdminSystem::getMode()      const { return mode; }
PullSize   AdminSystem::getPullSize()  const { return pullSize; }

std::vector<uint8_t> AdminSystem::generateRandom() {
    size_t bytesNeeded = static_cast<size_t>(pullSize);

    switch (mode) {
    case SourceMode::CPU_ONLY: {
        CPUEntropySource cpu;
        cpu.collectRawData(bytesNeeded);
        return SHA256::hash(cpu.getRawBytes());
    }
        case SourceMode::MIC_ONLY: {
        AudioEntropy mic(2, 500);  // фиксированные 500 мс
        mic.capture();

        if (!mic.hasData()) {
            std::cout << "Error: No data from microphone. Check if it's connected.\n";
            return {};
        }

        return SHA256::hash(mic.getCleanedBits());
        }
    case SourceMode::BOTH: {
        AudioEntropy mic(2, static_cast<DWORD>(bytesNeeded * 100));
        mic.capture();
        auto rawMic = mic.getCleanedBits();

        CPUEntropySource cpu;
        cpu.collectRawData(bytesNeeded);
        auto rawCpu = cpu.getRawBytes();

        return duo.mix(rawMic, rawCpu);
    }
    }
    return {};
}

std::string AdminSystem::generateRandomHex() {
    return DuoPull::toHexString(generateRandom());
}

// Преобразует вектор байтов в десятичную строку (большое число)
static std::string bytesToDecimalList(const std::vector<uint8_t>& bytes) {
    if (bytes.empty()) return "0";

    std::ostringstream oss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i > 0) oss << " ";
        oss << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

// ==================== INTERFACE ====================

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= min && val <= max) {
            clearInput();
            return val;
        }
        clearInput();
        std::cout << "Invalid input. Enter a number from " << min << " to " << max << ".\n";
    }
}

void AdminSystem::runInteractive() {
    while (true) {
        std::cout << "\n================ MAIN MENU ================\n";
        std::cout << "1 - Select source and pull size\n";
        std::cout << "2 - Generate random number\n";
        std::cout << "0 - Exit\n";
        std::cout << "============================================\n";

        int choice = readInt("Your choice: ", 0, 2);

        if (choice == 0) {
            std::cout << "Exiting.\n";
            break;
        }

        if (choice == 1) {
            std::cout << "\n--- Select source ---\n";
            std::cout << "1 - CPU only\n";
            std::cout << "2 - Microphone only\n";
            std::cout << "3 - Both\n";
            int src = readInt("Source: ", 1, 3);
            mode = static_cast<SourceMode>(src);

            std::cout << "\n--- Pull size (same for all sources) ---\n";
            std::cout << "1 - 64 bits (8 bytes)\n";
            std::cout << "2 - 128 bits (16 bytes)\n";
            std::cout << "3 - 256 bits (32 bytes)\n";
            std::cout << "4 - 512 bits (64 bytes)\n";
            std::cout << "5 - 1024 bits (128 bytes)\n";
            int ps = readInt("Pull size: ", 1, 5);
            PullSize sizes[] = { PullSize::BITS_64, PullSize::BITS_128,
                                 PullSize::BITS_256, PullSize::BITS_512,
                                 PullSize::BITS_1024 };
            pullSize = sizes[ps - 1];

            if (mode == SourceMode::BOTH) {
                std::cout << "\n--- Mixing method ---\n";
                std::cout << "1 - Mix raw then hash (RAW_THEN_HASH)\n";
                std::cout << "2 - Hash separately then XOR (HASH_THEN_MIX)\n";
                int mm = readInt("Method: ", 1, 2);
                MixMethod methods[] = { MixMethod::RAW_THEN_HASH, MixMethod::HASH_THEN_MIX };
                duo = DuoPull(pullSize, methods[mm - 1]);
            }

            std::cout << "\nSettings saved.\n";
            std::cout << "Source: ";
            if (mode == SourceMode::CPU_ONLY)      std::cout << "CPU only";
            else if (mode == SourceMode::MIC_ONLY) std::cout << "Microphone only";
            else                                    std::cout << "Both";
            std::cout << "\nPull size: " << (static_cast<int>(pullSize) * 8) << " bits ("
                      << static_cast<int>(pullSize) << " bytes)\n";
            if (mode == SourceMode::BOTH) {
                std::cout << "Mix method: ";
                if (duo.getMixMethod() == MixMethod::RAW_THEN_HASH)
                    std::cout << "RAW_THEN_HASH";
                else
                    std::cout << "HASH_THEN_MIX";
                std::cout << "\n";
            }
        }

        if (choice == 2) {
            std::cout << "\n--- Generation ---\n";
            auto bytes = generateRandom();
            std::cout << "Random number (hex): " << DuoPull::toHexString(bytes) << "\n";
            std::cout << "Random number (dec): " << bytesToDecimalList(bytes) << "\n";
            std::cout << "Length: " << bytes.size() << " bytes\n";
        }
    }
}
