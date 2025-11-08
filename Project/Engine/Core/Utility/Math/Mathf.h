#pragma once

/// std
#include <numbers>
#include <string>
#include <vector>

/// @brief fileの相対パスと名前のペア
using File = std::pair<std::string, std::string>;


/// //////////////////////////////////////////////////
/// よく使う関数やちょっと便利な関数をまとめた名前空間
/// //////////////////////////////////////////////////
namespace Mathf {

	static const float Deg2Rad = std::numbers::pi_v<float> / 180.0f;
	static const float Rad2Deg = 180.0f / std::numbers::pi_v<float>;
	static const float PI = std::numbers::pi_v<float>;

	float Cot(float _t);

	/// ----- ファイル ----- ///

	std::vector<File> FindFiles(const std::string& _fileDirectory, const std::string& _fileExtension);
	File FindFile(const std::string& _fileDirectory, const std::string& _filename);
	
	void ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to);

	/// 拡張子を除いたファイル名を取得
	std::string FileNameWithoutExtension(const std::string& _filename);

	/// 拡張子を取得
	std::string FileExtension(const std::string& _filename);

	/// @brief 指定されたファイルパスからCSVファイルを読み込み、2次元の整数ベクターとして返します。
	/// @param _filePath 読み込むCSVファイルのパス。
	/// @return CSVファイルの内容を表す2次元のint型std::vector。各内部ベクターはCSVの1行に対応します。
	std::vector<std::vector<int>> LoadCSV(const std::string& _filePath);

	/// @brief 指定した文字列が特定の接頭辞で始まっているかどうかを判定します。
	/// @param _str 判定対象となる文字列。
	/// @param _prefix 接頭辞として調べる文字列。
	/// @return 文字列が指定した接頭辞で始まっていれば true、そうでなければ false を返します。
	bool StartsWith(const std::string& _str, const std::string& _prefix);



	/// @brief 分子を分母で割り、余りがある場合は切り上げた商を返します。分母が0の場合の動作は未定義です。
	/// @param _numerator 割られる値（分子）。
	/// @param _denominator 割る値（分母）。0を渡してはなりません。
	/// @return 切り上げた商をUINT型で返します（余りがあれば上方向に丸められる）。
	uint32_t DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator);
}


