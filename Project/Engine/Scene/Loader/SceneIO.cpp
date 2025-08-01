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
	SaveScene(fileName_);

}

void SceneIO::Input(IScene* _scene) {
	/* jsonを読み込んでsceneに変換する */

	std::string type = typeid(*_scene).name();
	// "class "をstringから排除
	if (type.find("class ") == 0) {
		type = type.substr(6);
	}

	fileName_ = type + ".json";
	LoadScene(fileName_);

}

void SceneIO::OutputTemporary(IScene* _scene) {
	/* 一時的なシーンのjsonを保存する */

	std::string type = typeid(*_scene).name();
	// "class "をstringから排除
	if (type.find("class ") == 0) {
		type = type.substr(6);
	}
	fileName_ = type + "_temp.json";
	SaveScene(fileName_);

}

void SceneIO::InputTemporary(IScene* _scene) {
	std::string type = typeid(*_scene).name();
	// "class "をstringから排除
	if (type.find("class ") == 0) {
		type = type.substr(6);
	}
	fileName_ = type + "_temp.json";
	LoadScene(fileName_);

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
			Console::LogError("コンポーネントの追加に失敗しました: " + componentType);
		}
	}

}

void SceneIO::SaveScene(const std::string& _filename) {
	nlohmann::json outputJson = nlohmann::json::object();

	auto& entities = pECS_->GetEntities();
	for (auto& entity : entities) {
		/// マイナスIDはruntimeに生成されたエンティティなのでスキップ
		if (entity->GetId() < 0) {
			continue;
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

	std::unordered_map<uint32_t, IEntity*> entityMap;

	/// 実際にシーンに変換する
	for (const auto& entityJson : inputJson["entities"]) {
		const std::string& prefabName = entityJson["prefabName"];
		const std::string& entityClassName = entityJson["className"];
		const std::string& entityName = entityJson["name"];
		uint32_t entityId = entityJson["id"];

		IEntity* entity = nullptr;
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

		IEntity* entity = entityMap[entityId];
		if (entityJson.contains("parent") && !entityJson["parent"].is_null()) {
			int32_t parentId = entityJson["parent"];
			if (entityMap.find(parentId) != entityMap.end()) {
				entity->SetParent(entityMap[parentId]);
			}
		}
	}


	/// 全てのエンティティを初期化
	for (const auto& entityPair : entityMap) {
		IEntity* entity = entityPair.second;
		if (entity) {
			entity->Initialize();
		}
	}
}
