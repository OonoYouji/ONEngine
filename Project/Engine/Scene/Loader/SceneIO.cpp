#include "SceneIO.h"

/// std
#include <iostream>
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/EntityJsonConverter.h"
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
		nlohmann::json entityJson = EntityJsonConverter::ToJson(entity.get());
		if (entityJson.empty()) {
			continue; // エンティティの情報が空ならスキップ
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

	std::string type = typeid(*_scene).name();
	// "class "をstringから排除
	if (type.find("class ") == 0) {
		type = type.substr(6);
	}

	fileName_ = type + ".json";


	std::ifstream inputFile(fileDirectory_ + fileName_);
	if (!inputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + fileName_);
		return;
	}

	/// json形式に変換
	nlohmann::json inputJson;
	inputFile >> inputJson;
	inputFile.close();

	std::unordered_map<uint32_t, IEntity*> entityMap;

	/// 実際にシーンに変換する
	for (const auto& entityJson : inputJson["entities"]) {
		std::string entityName = entityJson["name"];
		uint32_t entityId = entityJson["id"];

		IEntity* entity = pECS_->GenerateEntity(entityName);
		if (entity) {
			LoadEntity(entityJson, entity);
			entityMap[entityId] = entity;
		}
	}


	/// エンティティの親子関係を設定
	for (const auto& entityJson : inputJson["entities"]) {
		uint32_t entityId = entityJson["id"];
		if (entityMap.find(entityId) == entityMap.end()) {
			continue; // エンティティが見つからない場合はスキップ
		}

		IEntity* entity = entityMap[entityId];
		if (entityJson.contains("parent") && !entityJson["parent"].is_null()) {
			uint32_t parentId = entityJson["parent"];
			if (entityMap.find(parentId) != entityMap.end()) {
				entity->SetParent(entityMap[parentId]);
			}
		}
	}

}

void SceneIO::LoadEntity(const nlohmann::json& _entityJson, IEntity* _entity) {

	/// コンポーネントを追加
	for (const auto& componentJson : _entityJson["components"]) {
		const std::string componentType = componentJson.at("type").get<std::string>();
		IComponent* comp = _entity->AddComponent(componentType);
		if (comp) {
			ComponentJsonConverter::FromJson(componentJson, comp);
			comp->SetOwner(_entity);

			if (componentType == "Variables") {
				Variables* vars = static_cast<Variables*>(comp);
				vars->LoadJson("./Assets/Jsons/" + _entity->GetName() + ".json");
			}

		} else {
			Console::Log("コンポーネントの追加に失敗しました: " + componentType);
		}
	}


}
