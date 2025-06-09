#include "LoadLevelTestScene.h"

/// std
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"
#include "Engine/ECS/Entity/Entities/Grid/Grid.h"

LoadLevelTestScene::LoadLevelTestScene() {
	loadResourcePaths_ = {
		"./Assets/Models/primitive/cube.obj",
	};
}

void LoadLevelTestScene::Initialize() {

	pEntityComponentSystem_->GenerateEntity<Grid>();

	LoadLevel("Assets/Levels/scene.json");
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

	file >> jsonData;

	for (const auto& objectData : jsonData["objects"]) {
		/// エンティティの名前を取得
		std::string objectName = objectData["name"].get<std::string>();
		/// エンティティのタイプを取得
		std::string objectType = objectData["type"].get<std::string>();
		
		if (!objectData.contains("transform") || objectData["transform"].is_null()) {
			Console::Log("transform not found");
			continue;
		}

		nlohmann::json transformData = objectData["transform"];
		Vec3 position = transformData["translation"].get<Vec3>();
		Vec3 rotate   = transformData["rotation"].get<Vec3>();
		Vec3 scale    = transformData["scale"].get<Vec3>();

		/// エンティティを生成&SRTを設定
		EmptyEntity* entity = pEntityComponentSystem_->GenerateEntity<EmptyEntity>();
		entity->SetPosition(position);
		entity->SetRotate(rotate);
		entity->SetScale(scale);
		entity->UpdateTransform();

		entity->SetName(objectName);

		if (objectType == "MESH") {
			MeshRenderer* meshRenderer = entity->AddComponent<MeshRenderer>();
			//meshRenderer->SetMesh(objectData["mesh"].get<std::string>());
		}
	}

}
