#include "FileSystem.h"

using namespace ONEngine;

/// std
#include <filesystem>
#include <fstream>

/// externals
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"

namespace fs = std::filesystem;


/**
 * @brief 指定したディレクトリ内で、指定した拡張子を持つファイルを再帰的に検索します。
 * @param _fileDirectory 検索対象のディレクトリパス
 * @param _fileExtension 探索対象の拡張子（例: ".png", ".fbx"）
 * @return 見つかったファイル（相対パスとファイル名のペア）のベクター
 */
std::vector<File> FileSystem::GetFiles(const std::string& _fileDirectory, const std::string& _fileExtension) {
	/// ----- 指定されたディレクトリ内のファイルを全て探索 ----- ///

	std::vector<File> result{};
	// ディレクトリが存在するか確認
	if (!fs::exists(_fileDirectory) || !fs::is_directory(_fileDirectory)) {
		return result; // 空のベクターを返す
	}


	/// 拡張子がある場合とない場合で処理を分ける
	if (_fileExtension.empty()) {

		/// ディレクトリ内のファイルを全て探索
		for (const auto& entry : fs::recursive_directory_iterator(_fileDirectory)) {
			if (fs::is_regular_file(entry)) {
				result.emplace_back(entry.path().string(), entry.path().filename().string());
			}
		}

	} else {

		/// 指定された拡張子を持つファイルのみを探索
		for (const auto& entry : fs::recursive_directory_iterator(_fileDirectory)) {
			if (fs::is_regular_file(entry) && entry.path().extension() == _fileExtension) {
				result.emplace_back(entry.path().string(), entry.path().filename().string());
			}
		}
	}

	for (auto& file : result) {
		ReplaceAll(&file.first, "\\", "/");
		ReplaceAll(&file.second, "\\", "/");
	}

	return result;
}

/**
 * @brief 指定したディレクトリ内で特定のファイル名を持つファイルを探索します。
 * @param _fileDirectory 探索対象のディレクトリパス
 * @param _filename 探索対象のファイル名
 * @return 見つかったファイル
 */
File FileSystem::GetFile(const std::string& _fileDirectory, const std::string& _filename) {
	// ディレクトリが存在するか確認
	if (!fs::exists(_fileDirectory) || !fs::is_directory(_fileDirectory)) {
		Console::LogError("Directory does not exist: " + _fileDirectory);
		return File(); // 空のFileを返す
	}

	/// ディレクトリ内のファイルを探索
	for (const auto& entry : fs::recursive_directory_iterator(_fileDirectory)) {
		if (fs::is_regular_file(entry) && entry.path().filename() == _filename) {
			std::string filePath = entry.path().string();
			ReplaceAll(&filePath, "\\", "/"); // パスの区切り文字を統一
			return File(filePath, entry.path().filename().string());
		}
	}

	return File();
}

/**
 * @brief 指定したディレクトリ内で特定のファイルが存在するか判定します。
 * @param _fileDirectory 探索対象のディレクトリパス
 * @param _filename 探索対象のファイル名
 * @return 存在すれば true、それ以外は false
 */
bool FileSystem::FileExists(const std::string& _fileDirectory, const std::string& _filename) {
	/// ディレクトリが存在するか確認
	if (!fs::exists(_fileDirectory) || !fs::is_directory(_fileDirectory)) {
		return false;
	}

	/// ディレクトリ内のファイルを探索、ファイル名が一致したらtrueを返す
	for (const auto& entry : fs::recursive_directory_iterator(_fileDirectory)) {
		if (fs::is_regular_file(entry) && entry.path().filename() == _filename) {
			return true;
		}
	}

	/// 見つからなかった場合
	return false;
}

/**
 * @brief 指定したファイルパスにファイルが存在するか判定します。
 * @param _path 探索するファイルのフルパスまたは相対パス
 * @return 存在すれば true、それ以外は false
 */
bool FileSystem::FileExists(const std::string& _path) {
	return std::filesystem::exists(_path);
}

/**
 * @brief 文字列内の特定の文字列パターンをすべて別の文字列に置換します（インプレース版）。
 * @param _str 変換対象の文字列ポインタ
 * @param _from 検索対象の置換前パターン
 * @param _to 置換後の文字列
 */
void FileSystem::ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to) {
	if (!_str) {
		return; // nullptrチェック
	}

	/// 対象が空なら何もしない
	if (_from.empty()) {
		return;
	}

	size_t pos = 0;
	while ((pos = _str->find(_from, pos)) != std::string::npos) {
		_str->replace(pos, _from.length(), _to);
		pos += _to.length(); // 次の検索位置を更新
	}
}

/**
 * @brief 文字列内の特定の文字列パターンをすべて別の文字列に置換した新しい文字列を生成します。
 * @param _str 対象の文字列
 * @param _from 置換前パターン
 * @param _to 置換後パターン
 * @return 置換後の新しい文字列
 */
std::string ONEngine::FileSystem::ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to) {
	std::string result = _str;
	ReplaceAll(&result, _from, _to);
	return result;
}

/**
 * @brief 引数のファイル名またはパスから拡張子を除いたファイル名部分を取得します。
 * @param _filename ファイル名またはフルパス
 * @return 拡張子なしのファイル名
 */
std::string FileSystem::FileNameWithoutExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return _filename;  // 拡張子がなければそのまま返す
	}
	return _filename.substr(0, lastDot);
}

/**
 * @brief ファイル名またはパスから拡張子部分（例: ".txt"）を取得します。
 * @param _filename ファイル名またはフルパス
 * @return 拡張子（ドットを含む）
 */
std::string FileSystem::FileExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return "";  // 拡張子がなければ空文字を返す
	}
	return _filename.substr(lastDot); // 拡張子を返す
}

/**
 * @brief 指定されたファイルパスからCSVファイルを読み込み、2次元の整数ベクターとして返します。
 * @param _filePath 読み込むCSVファイルのパス
 * @return 2次元の整数配列データ
 */
std::vector<std::vector<int>> FileSystem::LoadCSV(const std::string& _filePath) {
	/// ----- CSVファイルを読み込む ----- ///

	std::vector<std::vector<int>> data;

	/// ファイルを開く
	std::ifstream file(_filePath);
	if (!file.is_open()) {
		Console::LogError("Mathf::LoadCSV: Could not open file " + _filePath);
		return data; // 空のベクターを返す
	}

	/// 行ごとに読み込む
	std::string line;
	while (std::getline(file, line)) {
		std::vector<int> row;
		std::stringstream ss(line);
		std::string cell;

		while (std::getline(ss, cell, ',')) {
			try {
				int value = std::stoi(cell);
				row.push_back(value);
			} catch (const std::invalid_argument&) {
				Console::LogError("Mathf::LoadCSV: Invalid integer in file " + _filePath + ": " + cell);
			}
		}

		data.push_back(row);
	}

	file.close();
	return data;
}

/**
 * @brief 指定した文字列が特定の接頭辞で始まっているかどうかを判定します。
 * @param _str 判定対象の文字列
 * @param _prefix 調べる接頭辞
 * @return 接頭辞で始まっていれば true、そうでなければ false
 */
bool FileSystem::StartsWith(const std::string& _str, const std::string& _prefix) {
	return _str.rfind(_prefix, 0) == 0;
}


/**
 * @brief ディレクトリとファイル名を指定して、テキストファイルの内容を読み込みます。
 * @param _directory ファイルが存在するディレクトリ
 * @param _filename ファイル名
 * @return 読み込んだファイルの内容（テキスト）。失敗した場合は空文字列を返します。
 */
std::string FileSystem::LoadFile(const std::string& _directory, const std::string& _filename) {
	/// ----- ファイルを読み込む ----- ///

	if (!FileExists(_directory, _filename)) {
		return "";
	}

	/// パスをフルパスに変換
	std::filesystem::path dir(_directory);
	std::filesystem::path filename(_filename);
	std::filesystem::path fullPath = dir / filename;

	return LoadFile(fullPath.string());
}

/**
 * @brief ファイルパスを指定して、テキストファイルの内容を読み込みます。
 * @param _path 読み込むファイルのパス
 * @return 読み込んだファイルの内容（テキスト）。失敗した場合は空文字列を返します。
 */
std::string FileSystem::LoadFile(const std::string& _path) {
	/// ----- ファイルを読み込む ----- ///

	// ファイルストリームで読み込み
	std::ifstream file(_path);
	if (!file.is_open()) {
		return ""; // 開けなかった場合も空文字列
	}

	/// ファイルの中身をテキストに
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}



/**
 * @brief C#（Mono）側からファイルコンテンツを読み込むために呼び出されるバインド関数。
 * @param _path 読み込み対象のファイルパス（C#文字列）
 * @return 読み込んだファイルのテキスト内容（C#文字列ポインタ）
 */
MonoString* MonoInternalMethods::LoadFile(MonoString* _path) {

	/// スクリプト名をUTF-8に変換
	char* cstr = mono_string_to_utf8(_path);
	std::string pathStr(cstr);
	mono_free(cstr);

	std::string fileText = FileSystem::LoadFile(pathStr);
	MonoString* monoStr = mono_string_new(mono_domain_get(), fileText.c_str());

	return monoStr;
}
