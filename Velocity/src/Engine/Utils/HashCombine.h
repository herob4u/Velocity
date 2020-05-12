#pragma once
#include <stdint.h>

namespace
{
    inline void hash_combine(size_t& seed) { }

    template <typename T, typename... Rest>
    inline void hash_combine(size_t& seed, const T& v, Rest... rest) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        hash_combine(seed, rest...);
    }
}