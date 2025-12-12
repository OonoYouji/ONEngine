#pragma once

/// std
#include <string>
#include <vector>

/// externals
#include <mono/metadata/object.h>

namespace ONEngine {

/// @brief fileの相対パスと名前のペア
using File = std::pair<std::string, std::string>;

/// //////////////////////////////////////////////////////
/// ファイルの処理関数
/// //////////////////////////////////////////////////////
namespace FileSystem {

/// @brief ファイルの検索
/// @param _fileDirectory 検索対象のディレクトリ
/// @param _fileExtension 探索対象のファイル拡張子
/// @return 見つかったファイルのパスと名前のペアのベクター
std::vector<File> FindFiles(const std::string& _fileDirectory, const std::string& _fileExtension);

/// @brief ファイルの探索
/// @param _fileDirectory 探索対象のディレクトリ
/// @param _filename 探索対象のファイル名
/// @return 見つかったファイル
File GetFile(const std::string& _fileDirectory, const std::string& _filename);

/// @brief ファイルの存在確認
/// @param _fileDirectory 探索対象のディレクトリ
/// @param _filename 探索対象のファイル名
/// @return 見つかったファイル
bool FindFile(const std::string& _fileDirectory, const std::string& _filename);

/// @brief ファイルを探す
/// @param _path 探索するファイルのパス
/// @return true: 見つかった / false: 見つからなかった
bool FindFile(const std::string& _path);

/// @brief _str内の_allを_toに置換する
/// @param _str 変換対象の文字列ポインタ
/// @param _from 変換する文字列
/// @param _to 変換後の文字列
void ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to);

/// @brief stringないの_allを_toに置換した新しい文字列を返す
/// @param _str 対象の文字列
/// @param _from 変換する文字列
/// @param _to 変換後の文字列
/// @return 変換後の新しい文字列
std::string ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to);

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


/// @brief ファイルを読み込む
/// @param _directory ファイルディレクト
/// @param _filename ファイル名
/// @return 読み込んだファイルのテキスト内容、失敗したら空文字列
std::string LoadFile(const std::string& _directory, const std::string& _filename);

/// @brief ファイルを読み込む
/// @param _path 読み込むファイルのパス
/// @return 読み込んだファイルのテキスト内容、失敗したら空文字列
std::string LoadFile(const std::string& _path);

}


/// //////////////////////////////////////////////////////
/// Monoから呼び出すための内部関数群
/// //////////////////////////////////////////////////////
namespace MonoInternalMethods {

/// @brief ファイルを読み込む
MonoString* LoadFile(MonoString* _path);

}

} /// namespace ONEngine