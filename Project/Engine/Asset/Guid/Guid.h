#pragma once

/// std
#include <cstdint>
#include <string>
#include <functional>

/// externals
#include <nlohmann/json.hpp>



namespace ONEngine {
struct Guid;

/**
 * @brief nlohmann::jsonのデシリアライズ用関数。JSONからGuidへ変換します。
 * @param _j jsonオブジェクト
 * @param _guid 変換先のGuid参照
 */
void from_json(const nlohmann::json& _j, Guid& _guid);

/**
 * @brief nlohmann::jsonのシリアライズ用関数。GuidからJSONへ変換します。
 * @param _j jsonオブジェクト
 * @param _guid 変換元のGuid参照
 */
void to_json(nlohmann::json& _j, const Guid& _guid);

/**
 * @struct Guid
 * @brief 128ビット（64ビット整数2個）で表される一意な識別子（GUID）を管理する構造体
 */
struct Guid final {
	/**
	 * @brief デフォルトコンストラクタ。無効値（kInvalid）で初期化します。
	 */
	Guid();

	/**
	 * @brief 指定値によるコンストラクタ。
	 * @param _high 上位64ビット値
	 * @param _low 下位64ビット値
	 */
	Guid(uint64_t _high, uint64_t _low);

	/// ==================================================
	/// public : objects
	/// ==================================================

	uint64_t high; ///< 上位64ビットデータ
	uint64_t low;  ///< 下位64ビットデータ

	static const Guid kInvalid; ///< 無効なGUIDを表す定数オブジェクト


	/// ==================================================
	/// public : methods
	/// ==================================================

	/// ----- methods ----- ///

	/**
	 * @brief GUIDオブジェクトを文字列（32文字の16進数ハイフンなし形式）に変換します。
	 * @return 変換された文字列
	 */
	std::string ToString() const;

	/**
	 * @brief 有効なGUIDかどうか（無効値kInvalidでないか）を判定します。
	 * @return 有効な場合はtrue、無効な場合はfalse
	 */
	bool CheckValid() const;



	/// ----- static methods ----- ///

	/**
	 * @brief GUIDオブジェクトを文字列に変換するヘルパー静的メソッド。
	 * @param _guid 対象のGuidオブジェクト
	 * @return 変換された文字列
	 */
	static std::string ToString(const Guid& _guid);

	/**
	 * @brief 文字列形式（32文字16進数）からGUIDオブジェクトを再構成します。
	 * @param _str GUID文字列
	 * @return 再構成されたGuidオブジェクト
	 */
	static Guid FromString(const std::string& _str);

};


/// ----- operator ----- ///

/**
 * @brief 等価比較演算子。
 */
bool operator==(const Guid& a, const Guid& b);

/**
 * @brief 不等価比較演算子。
 */
bool operator!=(const Guid& a, const Guid& b);


/**
 * @brief 新しい一意なGUIDをランダム生成します（Windows API等を使用）。
 * @return 新しく生成されたGuidオブジェクト
 */
Guid GenerateGuid();

} /// ONEngine


/// @brief unordered_mapでGuidをキーとして使うためのハッシュ関数の特殊化
namespace std {
template<>
struct hash<ONEngine::Guid> {
	std::size_t operator()(const ONEngine::Guid& g) const noexcept {
		// 64bit × 2 → 1つのハッシュ値に圧縮
		// ここではXOR＋ビットシフトを利用（軽量で十分衝突率が低い）
		uint64_t h = g.high ^ (g.low + 0x9e3779b97f4a7c15ULL + (g.high << 6) + (g.high >> 2));
		return static_cast<std::size_t>(h);
	}
};
}

