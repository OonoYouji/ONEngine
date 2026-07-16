#pragma once

#include <string>
#include <cstdint>

namespace ONEngine {

/**
 * @class StringHash
 * @brief 文字列から高速に一意のハッシュ値を計算するユーティリティクラス
 */
class StringHash {
public:
    /**
     * @brief 与えられた文字列の FNV-1a ハッシュ値（32ビット）を計算します。
     * @param _str ハッシュ化する文字列
     * @return 計算されたハッシュ値 (uint32_t)
     */
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
