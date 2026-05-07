#include "generate_file.h"

void appendBits(const std::vector<uint8_t>& vec, const std::string& filename) {
    std::ofstream out(filename, std::ios::app);
    for (uint8_t byte : vec) {
        for (int i = 7; i >= 0; --i) {
            out << ((byte >> i) & 1);
        }
    }
}

void generate_file(int num) {
    std::ofstream init("output.txt", std::ios::trunc);
    init.close();

    AudioEntropy entropy(2, 500);
    for (int i = 0; i < num; ++i)
    {
        auto random = entropy.getRandom();
        appendBits(random, "output.txt");
    }
}