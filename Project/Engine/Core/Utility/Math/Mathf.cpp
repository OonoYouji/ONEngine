#include "Mathf.h"

/// std
#include <filesystem>
#include <fstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// namespaceの短縮
namespace fs = std::filesystem;


float Mathf::Cot(float _t) {
	return 1.0f / std::tan(_t);
}

std::vector<File> Mathf::FindFiles(const std::string& _fileDirectory, const std::string& _fileExtension) {
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

File Mathf::FindFile(const std::string& _fileDirectory, const std::string& _filename) {
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

void Mathf::ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to) {

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

std::string Mathf::FileNameWithoutExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return _filename;  // 拡張子がなければそのまま返す
	}
	return _filename.substr(0, lastDot);
}

std::string Mathf::FileExtension(const std::string& _filename) {
	size_t lastDot = _filename.find_last_of('.');
	if (lastDot == std::string::npos) {
		return "";  // 拡張子がなければ空文字を返す
	}
	return _filename.substr(lastDot); // 拡張子を返す
}

std::vector<std::vector<int>> Mathf::LoadCSV(const std::string& _filePath) {
	std::vector<std::vector<int>> data;

	std::ifstream file(_filePath);

	if (!file.is_open()) {
		Console::Log("[error] Mathf::LoadCSV: Could not open file " + _filePath);
		return data; // 空のベクターを返す
	}

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

bool Mathf::StartsWith(const std::string& _str, const std::string& _prefix) {
	return _str.rfind(_prefix, 0) == 0;
}
