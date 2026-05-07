#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include "audio_entropy.h"

void appendBits(const std::vector<uint8_t>& vec, const std::string& filename);
void generate_file(int num);