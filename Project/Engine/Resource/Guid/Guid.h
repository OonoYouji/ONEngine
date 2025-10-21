#pragma once

/// std
#include <cstdint>
#include <string>

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
