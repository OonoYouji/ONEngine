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

SceneIO::SceneIO(EntityComponentSystem* _ecs) : pEcs_(_ecs) {
	fileName_ = "";
	fileDirectory_ = "./Assets/Scene/";
}
SceneIO::~SceneIO() {}

void SceneIO::Output(const std::string& _sceneName, ECSGroup* _ecsGroup) {
	/* sceneをjsonに保存する */
	fileName_ = _sceneName + ".json";
	SaveScene(fileName_, _ecsGroup);
}

void SceneIO::Input(const std::string& _sceneName, ECSGroup* _ecsGroup) {
	/* jsonを読み込んでsceneに変換する */
	fileName_ = _sceneName + ".json";
	LoadScene(fileName_, _ecsGroup);
}

void SceneIO::OutputTemporary(ECSGroup* _ecsGroup) {
	tempSceneJson_.clear();
	SaveSceneToJson(tempSceneJson_, _ecsGroup);
}

void SceneIO::InputTemporary(ECSGroup* _ecsGroup) {
	LoadSceneFromJson(tempSceneJson_, _ecsGroup);
}

void SceneIO::SaveScene(const std::string& _filename, ECSGroup* _ecsGroup) {
	nlohmann::json outputJson = nlohmann::json::object();
	SaveSceneToJson(outputJson, _ecsGroup);
	OutputJson(outputJson, _filename);
}

void SceneIO::LoadScene(const std::string& _filename, ECSGroup* _ecsGroup) {
	std::ifstream inputFile(fileDirectory_ + _filename);
	if (!inputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + _filename);
		return;
	}

	/// json形式に変換
	nlohmann::json inputJson;
	inputFile >> inputJson;
	inputFile.close();

	LoadSceneFromJson(inputJson, _ecsGroup);
}

void SceneIO::SaveSceneToJson(nlohmann::json& _output, ECSGroup* _ecsGroup) {

	auto& entities = _ecsGroup->GetEntities();
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

		_output["entities"].push_back(entityJson);
	}

}

void SceneIO::LoadSceneFromJson(const nlohmann::json& _input, ECSGroup* _ecsGroup) {
	std::unordered_map<uint32_t, GameEntity*> entityMap;

	if (!_input.contains("entities")) {
		return;
	}

	/// 実際にシーンに変換する
	for (const auto& entityJson : _input["entities"]) {
		const std::string& prefabName = entityJson.value("prefabName", "");
		const std::string& entityName = entityJson.value("name", "");
		const uint32_t entityId = entityJson.value("id", 0);

		GameEntity* entity = nullptr;
		if (!prefabName.empty()) {
			std::string jsonPrefabName = entityJson["prefabName"];
			entity = _ecsGroup->GenerateEntityFromPrefab(jsonPrefabName, false);
		} else {
			entity = _ecsGroup->GenerateEntity(false);
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
	for (const auto& entityJson : _input["entities"]) {
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

void SceneIO::OutputJson(const nlohmann::json& _json, const std::string& _filename) {
	/// ファイルが無かったら生成する
	if (!std::filesystem::exists(fileDirectory_ + _filename)) {
		std::filesystem::create_directories(fileDirectory_);
	}

	/// ファイルに保存する
	std::ofstream outputFile(fileDirectory_ + _filename);
	if (!outputFile.is_open()) {
		Console::LogError("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + _filename);
	}

	outputFile << _json.dump(4);
	outputFile.close();
}
