#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "DuoPull.h"

enum class SourceMode {
    CPU_ONLY = 1,
    MIC_ONLY = 2,
    BOTH     = 3
};

class AdminSystem {
private:
    SourceMode mode;
    DuoPull    duo;
    PullSize   pullSize;

public:
    AdminSystem();

    void setMode(SourceMode m);
    void setPullSize(PullSize size);
    void setDuoConfig(PullSize size, MixMethod method);

    SourceMode getMode()      const;
    PullSize   getPullSize()  const;

    std::vector<uint8_t> generateRandom();
    std::string          generateRandomHex();

    void runInteractive();
};