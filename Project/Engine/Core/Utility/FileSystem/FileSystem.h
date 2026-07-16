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

/**
 * @brief 指定したディレクトリ内で、指定した拡張子を持つファイルを再帰的に検索します。
 * @param _fileDirectory 検索対象のディレクトリパス
 * @param _fileExtension 探索対象の拡張子（例: ".png", ".fbx"）
 * @return 見つかったファイル（相対パスとファイル名のペア）のベクター
 */
std::vector<File> GetFiles(const std::string& _fileDirectory, const std::string& _fileExtension);

/**
 * @brief 指定したディレクトリ内で特定のファイル名を持つファイルを探索します。
 * @param _fileDirectory 探索対象のディレクトリパス
 * @param _filename 探索対象のファイル名
 * @return 見つかったファイル（見つからない場合は空のペア）
 */
File GetFile(const std::string& _fileDirectory, const std::string& _filename);

/**
 * @brief 指定したディレクトリ内で特定のファイルが存在するか判定します。
 * @param _fileDirectory 探索対象のディレクトリパス
 * @param _filename 探索対象のファイル名
 * @return 存在すれば true、それ以外は false
 */
bool FileExists(const std::string& _fileDirectory, const std::string& _filename);

/**
 * @brief 指定したファイルパスにファイルが存在するか判定します。
 * @param _path 探索するファイルのフルパスまたは相対パス
 * @return 存在すれば true、それ以外は false
 */
bool FileExists(const std::string& _path);

/**
 * @brief 文字列内の特定の文字列パターンをすべて別の文字列に置換します（インプレース版）。
 * @param _str 変換対象の文字列ポインタ
 * @param _from 検索対象の置換前パターン
 * @param _to 置換後の文字列
 */
void ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to);

/**
 * @brief 文字列内の特定の文字列パターンをすべて別の文字列に置換した新しい文字列を生成します。
 * @param _str 対象の文字列
 * @param _from 置換前パターン
 * @param _to 置換後パターン
 * @return 置換後の新しい文字列
 */
std::string ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to);

/**
 * @brief 引数のファイル名またはパスから拡張子を除いたファイル名部分を取得します。
 * @param _filename ファイル名またはフルパス
 * @return 拡張子なしのファイル名
 */
std::string FileNameWithoutExtension(const std::string& _filename);

/**
 * @brief ファイル名またはパスから拡張子部分（例: ".txt"）を取得します。
 * @param _filename ファイル名またはフルパス
 * @return 拡張子（ドットを含む）
 */
std::string FileExtension(const std::string& _filename);

/**
 * @brief 指定されたファイルパスからCSVファイルを読み込み、2次元の整数ベクターとして返します。
 * @param _filePath 読み込むCSVファイルのパス
 * @return 2次元の整数配列データ。各行が1次元配列として格納されます。
 */
std::vector<std::vector<int>> LoadCSV(const std::string& _filePath);

/**
 * @brief 指定した文字列が特定の接頭辞で始まっているかどうかを判定します。
 * @param _str 判定対象の文字列
 * @param _prefix 調べる接頭辞
 * @return 接頭辞で始まっていれば true、そうでなければ false
 */
bool StartsWith(const std::string& _str, const std::string& _prefix);


/**
 * @brief ディレクトリとファイル名を指定して、テキストファイルの内容を読み込みます。
 * @param _directory ファイルが存在するディレクトリ
 * @param _filename ファイル名
 * @return 読み込んだファイルの内容（テキスト）。失敗した場合は空文字列を返します。
 */
std::string LoadFile(const std::string& _directory, const std::string& _filename);

/**
 * @brief ファイルパスを指定して、テキストファイルの内容を読み込みます。
 * @param _path 読み込むファイルのパス
 * @return 読み込んだファイルの内容（テキスト）。失敗した場合は空文字列を返します。
 */
std::string LoadFile(const std::string& _path);

}


/// //////////////////////////////////////////////////////
/// Monoから呼び出すための内部関数群
/// //////////////////////////////////////////////////////
namespace MonoInternalMethods {

/**
 * @brief C#（Mono）側からファイルコンテンツを読み込むために呼び出されるバインド関数。
 * @param _path 読み込み対象のファイルパス（C#文字列）
 * @return 読み込んだファイルのテキスト内容（C#文字列ポインタ）
 */
MonoString* LoadFile(MonoString* _path);

}

} /// namespace ONEngine