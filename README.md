# CSPRNG — Cryptographically Stable Pseudo-Random Number Generator

Генератор криптографически стойких случайных чисел на основе физических источников энтропии: **шума микрофона** и **джиттера процессора**.

---

## Источники энтропии

| Источник | Принцип |
|----------|---------|
| AudioEntropy | Тепловой шум АЦП звуковой карты |
| CPUEntropySource | Джиттер процессора (RDTSC) |

---

## Возможности

- Выбор источника: только микрофон, только процессор, оба вместе
- Выбор размера порции сырых битов: 64, 128, 256, 512, 1024 бит
- Два метода смешивания: RAW_THEN_HASH / HASH_THEN_MIX
- Вывод случайного числа в hex и decimal (побайтово)
- Хеширование SHA-256 на финальном этапе

---

## Структура проекта
├── include/  
│ ├── AudioEntropy.h  
│ ├── CPUEntropySource.h  
│ ├── EntropyPool.h  
│ └── SHA256.h  
├── includeDUO/  
│ ├── AdminSystem.h  
│ └── DuoPull.h  
├── src/  
│ ├── AudioEntropy.cpp  
│ ├── CPUEntropySource.cpp  
│ ├── EntropyPool.cpp  
│ └── SHA256.cpp  
├── srcDUO/  
│ ├── AdminSystem.cpp  
│ └── DuoPull.cpp  
├── main.cpp  
└── CMakeLists.txt  

---

## Сборка

Требуется **CMake 3.10+**, компилятор с поддержкой **C++11**, **Windows** (для AudioEntropy).

## Использование  
При запуске открывается консольное меню:
  
1 - Select source and pull size  
2 - Generate random number  
0 - Exit  
  
## Пример вывода  
  
Random number (hex): 49 fb 16 50 0a 25 02 bf 8d 9a 1b c5 ...  
Random number (dec): 73 251 22 80 10 37 2 191 141 154 27 197 ...  
Length: 32 bytes  
