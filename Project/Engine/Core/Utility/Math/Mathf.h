#pragma once

/// std
#include <numbers>
#include <string>
#include <vector>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"

namespace ONEngine {

/// @brief fileの相対パスと名前のペア
using File = std::pair<std::string, std::string>;


/// //////////////////////////////////////////////////
/// よく使う関数やちょっと便利な関数をまとめた名前空間
/// //////////////////////////////////////////////////
namespace Mathf {

static const float Deg2Rad = std::numbers::pi_v<float> / 180.0f;
static const float Rad2Deg = 180.0f / std::numbers::pi_v<float>;
static const float PI = std::numbers::pi_v<float>;


/// @brief 逆タンジェント
/// @param _t 
/// @return 
float Cot(float _t);

/*
* 8ing
* ファイル操作系の関数がここにあるのはおかしい、namespace FileSystem に作るべし
*/

/// ----- ファイル ----- ///

/// @brief ファイルの検索
/// @param _fileDirectory 検索対象のディレクトリ
/// @param _fileExtension 探索対象のファイル拡張子
/// @return 見つかったファイルのパスと名前のペアのベクター
std::vector<File> FindFiles(const std::string& _fileDirectory, const std::string& _fileExtension);

/// @brief ファイルの探索
/// @param _fileDirectory 探索対象のディレクトリ
/// @param _filename 探索対象のファイル名
/// @return 見つかったファイル
File FindFile(const std::string& _fileDirectory, const std::string& _filename);


/// @brief _str内の_allを_toに置換する
/// @param _str 変換対象の文字列ポインタ
/// @param _from 変換する文字列
/// @param _to 変換後の文字列
void ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to);



/// @brief 引数のファイル名から拡張子を除いた名前を取得
/// @param _filename ファイル名
/// @return ファイル名
std::string FileNameWithoutExtension(const std::string& _filename);

/// @brief ファイルの拡張子を取得
/// @param _filename ファイル名
/// @return 拡張子
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
/// @return 切り上げた商をuint32_t型で返します（余りがあれば上方向に丸められる）。
uint32_t DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator);

/// @brief _tを使い p0~p3 のCatmullRom補間位置を計算する
/// @param _p0 制御点0
/// @param _p1 制御点1
/// @param _p2 制御点2
/// @param _p3 制御点3
/// @param _t 補間パラメータ(0~1)
/// @return p0~p3のCatmullRom補間位置
Vector3 CatmullRomPosition(
	const Vector3& _p0, const Vector3& _p1,
	const Vector3& _p2, const Vector3& _p3,
	float _t
);

} /// namespace Mathf

} /// namespace ONEngine