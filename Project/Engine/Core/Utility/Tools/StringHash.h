#pragma once

#include <string>
#include <cstdint>

namespace ONEngine {

class StringHash {
public:
    static uint32_t Get(const std::string& _str) {
        // FNV-1a hash
        uint32_t hash = 2166136261u;
        for (char c : _str) {
            hash ^= static_cast<uint32_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }
};

} // namespace ONEngine
