#include "Mathf.h"

/// std
#include <filesystem>

/// namespaceの短縮
namespace fs = std::filesystem;


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
