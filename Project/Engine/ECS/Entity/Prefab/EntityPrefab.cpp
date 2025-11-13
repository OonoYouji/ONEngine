#include "EntityPrefab.h"

/// std
#include <filesystem>
#include <fstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

EntityPrefab::EntityPrefab(const std::string& _path)
	: path_(_path) {

	/* --- pathのJSONを読む --- */

	/// pathが存在するかチェック
	if (!std::filesystem::exists(path_)) {
		Console::LogWarning("Prefab path does not exist: " + path_);
		return;
	}


	/// JSONファイルを読み込む
	std::ifstream file(path_);
	if (!file.is_open()) {
		Console::LogError("Failed to open prefab file: " + path_);
		return;
	}

	/// jsonに変換、ファイルを閉じる
	file >> json_;
	file.close();
}

EntityPrefab::~EntityPrefab() {}

void EntityPrefab::Reload() {
	/* --- pathのJSONを読む --- */

	/// pathが存在するかチェック
	if (!std::filesystem::exists(path_)) {
		Console::LogWarning("Prefab path does not exist: " + path_);
		return;
	}


	/// JSONファイルを読み込む
	std::ifstream file(path_);
	if (!file.is_open()) {
		Console::LogError("Failed to open prefab file: " + path_);
		return;
	}

	/// jsonに変換、ファイルを閉じる
	file >> json_;
	file.close();
}
