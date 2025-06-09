#include "LoadLevelTestScene.h"

/// std
#include <fstream>

/// external
#include <nlohmann/json.hpp>


void LoadLevelTestScene::Initialize() {

}

void LoadLevelTestScene::Update() {

}

void LoadLevelTestScene::LoadLevel(const std::string& _levelName) {

	/// ファイル名が空ならば終了
	if (_levelName.empty()) {
		Console::Log("Level name is empty.");
		return;
	}

	/// ファイル名が存在しないならば終了
	if (!std::filesystem::exists(_levelName)) {
		Console::Log("Level file does not exist: " + _levelName);
		return;
	}

	/// fileを開く
	std::ifstream file(_levelName);
	if (!file.is_open()) {
		Console::Log("Failed to open level file: " + _levelName);
		return;
	}


	/// jsonを読み込む
	nlohmann::json jsonData;

	
}
