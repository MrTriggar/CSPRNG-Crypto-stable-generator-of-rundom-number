#include <iostream>
#include <iomanip>
#include "audio_entropy_macos.h"

int main()
{
    try
    {
        std::cout << "Creating AudioEntropy instance...\n";
        AudioEntropy entropy(2, 1000);  // 2 бита, 1 секунда захвата

        // 1. Проверим сырые семплы
        entropy.getRandom(); // вызов capture внутри
        auto raw = entropy.getRawSamples();
        std::cout << "Raw samples count: " << raw.size() << "\n";
        if (!raw.empty())
        {
            std::cout << "First 10 raw samples: ";
            for (size_t i = 0; i < 10 && i < raw.size(); ++i)
                std::cout << raw[i] << " ";
            std::cout << "\n";
        }

        // 2. Извлечённые биты (распакованные)
        auto bits = entropy.getExtractBits();
        std::cout << "Extracted bits count: " << bits.size() << "\n";
        if (!bits.empty())
        {
            std::cout << "First 20 bits: ";
            for (size_t i = 0; i < 20 && i < bits.size(); ++i)
                std::cout << (int)bits[i] << " ";
            std::cout << "\n";
        }

        // 3. Упакованные байты
        auto cleaned = entropy.getCleanedBits();
        std::cout << "Cleaned bytes count: " << cleaned.size() << "\n";

        // 4. Финальный хеш
        for (int i = 0; i < 5; ++i)
        {
            auto hash = entropy.getRandom(); // 32 байта
            std::cout << "Random hash " << i + 1 << ": ";
            for (unsigned char b : hash)
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
            std::cout << std::dec << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}