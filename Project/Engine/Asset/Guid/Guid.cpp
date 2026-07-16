#include "Guid.h"

/// std
#include <iomanip>
#include <sstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

namespace {

/// @brief stirngに変換したときにGuidの長さ
constexpr size_t kGuidStringLength = 32;

/// @brief high/lowそれぞれの16進数表記の長さ
constexpr size_t kGuidHexSegmentLength = 16;

} /// namespace


/**
 * @brief nlohmann::jsonのデシリアライズ用関数。
 * @param _j jsonオブジェクト
 * @param _guid 変換先のGuid参照
 */
void ONEngine::from_json(const nlohmann::json& _j, Guid& _guid) {
    _guid = Guid::FromString(_j.get<std::string>());
}

/**
 * @brief nlohmann::jsonのシリアライズ用関数。
 * @param _j jsonオブジェクト
 * @param _guid 変換元のGuid参照
 */
void ONEngine::to_json(nlohmann::json& _j, const Guid& _guid) {
    _j = _guid.ToString();
}


/// @brief 無効値のGuid定義
const Guid Guid::kInvalid = Guid{ 0, 0 };


/**
 * @brief デフォルトコンストラクタ
 */
Guid::Guid() : high(0), low(0) {}

/**
 * @brief 指定値によるコンストラクタ
 * @param _high 上位64ビット値
 * @param _low 下位64ビット値
 */
Guid::Guid(uint64_t _high, uint64_t _low) : high(_high), low(_low) {}

/**
 * @brief GUIDオブジェクトを文字列（32文字の16進数ハイフンなし形式）に変換します。
 * @return 変換された文字列
 */
std::string Guid::ToString() const {
	/// ----- GuidをStringに変換する ----- ///

	std::ostringstream oss;
	oss << std::hex << std::setfill('0')
		<< std::setw(kGuidHexSegmentLength) << high
		<< std::setw(kGuidHexSegmentLength) << low;
	return oss.str();
}

/**
 * @brief 有効なGUIDかどうかをチェックします。
 * @return 有効な場合はtrue
 */
bool Guid::CheckValid() const {
	return (high != 0) || (low != 0);
}

/**
 * @brief GUIDオブジェクトを文字列に変換するヘルパー静的メソッド。
 * @param _guid 対象のGuid
 * @return 変換された文字列
 */
std::string Guid::ToString(const Guid& _guid) {
	return _guid.ToString();
}

/**
 * @brief 文字列形式からGUIDオブジェクトを再構成します。
 * @param _str GUID文字列
 * @return 再構成されたGuidオブジェクト
 */
Guid Guid::FromString(const std::string& _str) {
	/// ----- StringをGuidに変換して返す ----- ///

	/// _str が32文字でない場合は無効なGuidを返す
	if (_str.size() != kGuidStringLength) {
		return Guid{};
	}

	uint64_t hi = std::stoull(_str.substr(0, kGuidHexSegmentLength), nullptr, kGuidHexSegmentLength);
	uint64_t lo = std::stoull(_str.substr(kGuidHexSegmentLength, kGuidHexSegmentLength), nullptr, kGuidHexSegmentLength);
	return Guid(hi, lo);
}

/**
 * @brief 等価比較演算子。
 */
bool ONEngine::operator==(const Guid& a, const Guid& b) {
	return (a.high == b.high) && (a.low == b.low);
}

/**
 * @brief 不等価比較演算子。
 */
bool ONEngine::operator!=(const Guid& a, const Guid& b) {
	return !(a == b);
}

/**
 * @brief 新しい一意なGUIDを乱数によって生成します。
 * @return 生成されたGuidオブジェクト
 */
Guid ONEngine::GenerateGuid() {
	return Guid(Random::UInt64(), Random::UInt64());
}
