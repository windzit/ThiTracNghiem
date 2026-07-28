#pragma once

namespace Utility {

template <typename T>
void Swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

} // namespace Utility
