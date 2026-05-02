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

bool FileSystem::FileExists(const std::string& _path) {
	return std::filesystem::exists(_path);
}

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

std::string ONEngine::FileSystem::ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to) {
	std::string result = _str;
	ReplaceAll(&result, _from, _to);
	return result;
}

std::string FileSystem::FileNameWithoutExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return _filename;  // 拡張子がなければそのまま返す
	}
	return _filename.substr(0, lastDot);
}

std::string FileSystem::FileExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return "";  // 拡張子がなければ空文字を返す
	}
	return _filename.substr(lastDot); // 拡張子を返す
}

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

bool FileSystem::StartsWith(const std::string& _str, const std::string& _prefix) {
	return _str.rfind(_prefix, 0) == 0;
}


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



MonoString* MonoInternalMethods::LoadFile(MonoString* _path) {

	/// スクリプト名をUTF-8に変換
	char* cstr = mono_string_to_utf8(_path);
	std::string pathStr(cstr);
	mono_free(cstr);

	std::string fileText = FileSystem::LoadFile(pathStr);
	MonoString* monoStr = mono_string_new(mono_domain_get(), fileText.c_str());

	return monoStr;
}
