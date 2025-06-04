#include "SceneIO.h"

/// std
#include <iostream>
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"


SceneIO::SceneIO(EntityComponentSystem* _ecs)
	: pECS_(_ecs) {
	fileName_ = "";
	fileDirectory_ = "./Assets/Scene/";
}
SceneIO::~SceneIO() {}

void SceneIO::Output(IScene* _scene) {
	/* sceneをjsonに保存する */

	std::string type = typeid(*_scene).name();
	// "class "をstringから排除
	if (type.find("class ") == 0) {
		type = type.substr(6);
	}

	fileName_ = type + ".json";

	nlohmann::json outputJson = nlohmann::json::object();

	auto& entities = pECS_->GetEntities();
	for (auto& entity : entities) {
		nlohmann::json entityJson = nlohmann::json::object();
		entityJson["name"] = entity->GetName();

		// ここでコンポーネントの情報を追加する
		auto& components = entity->GetComponents();
		for (const auto& component : components) {
			entityJson["components"].push_back(ComponentJsonConverter::ToJson(component.second));
		}

		outputJson["entities"].push_back(entityJson);
	}


	/// ファイルが無かったら生成する
	if (!std::filesystem::exists(fileDirectory_ + fileName_)) {
		std::filesystem::create_directories(fileDirectory_);
	}
	

	std::ofstream outputFile(fileDirectory_ + fileName_);
	if (!outputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + fileName_);
	}

	outputFile << outputJson.dump(4);
	outputFile.close();

}

void SceneIO::Input(IScene* _scene) {
	/* jsonを読み込んでsceneに変換する */

	std::ifstream inputFile(fileDirectory_ + fileName_);
	if (!inputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + fileName_);
		return;
	}

	/// json形式に変換
	nlohmann::json inputJson;
	inputFile >> inputJson;
	inputFile.close();


	/// 実際にシーンに変換する
	for (const auto& entity : inputJson["entities"]) {
		std::string entityName = entity["name"];
		auto newEntity = pECS_->GenerateEntity(entityName);

	}



}
