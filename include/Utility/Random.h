#pragma once
#include <cstdlib>
#include <ctime>

namespace Utility {
namespace Random {

inline void Seed() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

inline int RandomInt(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + std::rand() % (max - min + 1);
}

} // namespace Random
} // namespace Utility
