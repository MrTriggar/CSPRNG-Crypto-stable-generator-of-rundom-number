#include <iostream>
#include <iomanip>
#include "audio_entropy.h"

using namespace std;
int main() {
    AudioEntropy entropy(2, 500);
    
    for (int i = 0; i < 10; ++i) {
        auto random = entropy.getRandom();
        std::cout << "Random " << i + 1 << ": ";
        for (uint8_t b : random)
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
        std::cout << std::dec << '\n';
    }
    return 0;
}