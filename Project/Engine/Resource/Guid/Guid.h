#pragma once

/// std
#include <cstdint>
#include <string>
#include <functional>

/// ////////////////////////////////////////////////////
/// GUID 構造体
/// ////////////////////////////////////////////////////
struct Guid final {

	Guid();
	Guid(uint64_t _high, uint64_t _low);

	/// ==================================================
	/// public : objects
	/// ==================================================

	uint64_t high;
	uint64_t low;


	/// ==================================================
	/// public : methods
	/// ==================================================

	/// ----- methods ----- ///

	/// @brief Guidを文字列に変換する
	std::string ToString() const;



	/// ----- static methods ----- ///

	/// @brief 文字列からGuidを生成する
	static std::string ToString(const Guid& _guid);

	/// @brief 文字列からGuidを生成する
	static Guid FromString(const std::string& _str);

};


/// ----- operator ----- ///
bool operator==(const Guid& a, const Guid& b);
bool operator!=(const Guid& a, const Guid& b);


/// @brief 新しいGuidを生成する
Guid GenerateGuid();


/// @brief unordered_mapでGuidをキーとして使うためのハッシュ関数の特殊化
namespace std {
	template<>
	struct hash<Guid> {
		std::size_t operator()(const Guid& g) const noexcept {
			// 64bit × 2 → 1つのハッシュ値に圧縮
			// ここではXOR＋ビットシフトを利用（軽量で十分衝突率が低い）
			uint64_t h = g.high ^ (g.low + 0x9e3779b97f4a7c15ULL + (g.high << 6) + (g.high >> 2));
			return static_cast<std::size_t>(h);
		}
	};
}
