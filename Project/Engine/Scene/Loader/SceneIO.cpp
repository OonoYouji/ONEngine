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

void SceneIO::Output(const std::string& _sceneName) {
	/* sceneをjsonに保存する */
	fileName_ = _sceneName + ".json";
	SaveScene(fileName_);
}

void SceneIO::Input(const std::string& _sceneName) {
	/* jsonを読み込んでsceneに変換する */
	fileName_ = _sceneName + ".json";
	LoadScene(fileName_);
}

void SceneIO::OutputTemporary(const std::string& _sceneName) {
	/* 一時的なシーンのjsonを保存する */
	fileName_ = _sceneName + "_temp.json";
	SaveScene(fileName_);
}

void SceneIO::InputTemporary(const std::string& _sceneName) {
	fileName_ = _sceneName + "_temp.json";
	LoadScene(fileName_);
}

void SceneIO::SaveScene(const std::string& _filename) {
	nlohmann::json outputJson = nlohmann::json::object();

	auto& entities = pECS_->GetEntities();
	for (auto& entity : entities) {
		/// マイナスIDはruntimeに生成されたエンティティなのでスキップ
		if (entity->GetId() < 0) {
			continue;
		}

		if (Variables* var = entity->GetComponent<Variables>()) {
			var->SaveJson("./Assets/Jsons/" + entity->GetName() + ".json");
		}

		nlohmann::json entityJson = EntityJsonConverter::ToJson(entity.get());
		if (entityJson.empty()) {
			continue; // エンティティの情報が空ならスキップ
		}

		outputJson["entities"].push_back(entityJson);
	}


	/// ファイルが無かったら生成する
	if (!std::filesystem::exists(fileDirectory_ + _filename)) {
		std::filesystem::create_directories(fileDirectory_);
	}


	std::ofstream outputFile(fileDirectory_ + _filename);
	if (!outputFile.is_open()) {
		Console::LogError("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + _filename);
	}

	outputFile << outputJson.dump(4);
	outputFile.close();
}

void SceneIO::LoadScene(const std::string& _filename) {

	std::ifstream inputFile(fileDirectory_ + _filename);
	if (!inputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + _filename);
		return;
	}

	/// json形式に変換
	nlohmann::json inputJson;
	inputFile >> inputJson;
	inputFile.close();

	std::unordered_map<uint32_t, GameEntity*> entityMap;

	/// 実際にシーンに変換する
	for (const auto& entityJson : inputJson["entities"]) {
		const std::string& prefabName = entityJson["prefabName"];
		const std::string& entityClassName = entityJson["className"];
		const std::string& entityName = entityJson["name"];
		uint32_t entityId = entityJson["id"];

		GameEntity* entity = nullptr;
		if (!prefabName.empty()) {
			std::string jsonPrefabName = entityJson["prefabName"];
			entity = pECS_->GenerateEntityFromPrefab(jsonPrefabName, false);
		} else {
			entity = pECS_->GenerateEntity(entityClassName, false);
		}

		if (entity) {
			entity->SetPrefabName(prefabName);
			entity->SetName(entityName);

			/// prefabがないならシーンに保存されたjsonからエンティティを復元
			if (prefabName.empty()) {
				EntityJsonConverter::FromJson(entityJson, entity);
			} else {
				EntityJsonConverter::TransformFromJson(entityJson, entity);
				if (Variables* vars = entity->GetComponent<Variables>()) {
					vars->LoadJson("./Assets/Jsons/" + entityName + ".json");
				}
			}

			entityMap[entityId] = entity;
		}
	}


	/// エンティティの親子関係を設定
	for (const auto& entityJson : inputJson["entities"]) {
		int32_t entityId = entityJson["id"];
		if (entityMap.find(entityId) == entityMap.end()) {
			continue; // エンティティが見つからない場合はスキップ
		}

		GameEntity* entity = entityMap[entityId];
		if (entityJson.contains("parent") && !entityJson["parent"].is_null()) {
			int32_t parentId = entityJson["parent"];
			if (entityMap.find(parentId) != entityMap.end()) {
				entity->SetParent(entityMap[parentId]);
			}
		}
	}

}
