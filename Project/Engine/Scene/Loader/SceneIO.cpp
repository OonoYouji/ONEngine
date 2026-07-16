#include "SceneIO.h"

using namespace ONEngine;

/// std
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <filesystem>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/Core/Utility/FileSystem/FileSystem.h"

/**
 * @brief コンストラクタ。シーンの保存先ディレクトリを設定します。
 * @param _ecs ECS管理クラスへのポインタ
 */
SceneIO::SceneIO(EntityComponentSystem* _ecs) : pEcs_(_ecs) {
	fileName_ = "";
	fileDirectory_ = "./Assets/Scene/";
}
/**
 * @brief デストラクタ。
 */
SceneIO::~SceneIO() {}

/**
 * @brief 指定したECSグループのエンティティデータをシーンファイルに出力します。
 * @param _sceneName 保存するシーン名
 * @param _ecsGroup 保存対象のECSグループ
 */
void SceneIO::Output(const std::string& _sceneName, ECSGroup* _ecsGroup) {
	fileName_ = _sceneName + ".scene";
	SaveScene(fileName_, _ecsGroup);
}

/**
 * @brief 指定したシーンファイルを読み込み、エンティティを生成してECSグループに復元します。
 * @param _sceneName 読み込むシーン名
 * @param _ecsGroup 復元対象のECSグループ
 */
void SceneIO::Input(const std::string& _sceneName, ECSGroup* _ecsGroup) {
	fileName_ = _sceneName + ".scene";
	LoadScene(fileName_, _ecsGroup);
}

/**
 * @brief シーンのエンティティデータをメモリ上のテンポラリ領域に一時保存します。
 * @param _ecsGroup 対象のECSグループ
 */
void SceneIO::OutputTemporary(ECSGroup* _ecsGroup) {
	tempSceneJson_.clear();
	SaveSceneToJson(tempSceneJson_, _ecsGroup);
}

/**
 * @brief メモリ上に一時保存されたデータからECSグループを復元します。
 * @param _ecsGroup 対象のECSグループ
 */
void SceneIO::InputTemporary(ECSGroup* _ecsGroup) {
	MonoScriptEngine::GetInstance().ClearECSGroup(_ecsGroup->GetGroupName());
	LoadSceneFromJson(tempSceneJson_, _ecsGroup);
}

/**
 * @brief シーンファイルおよび各エンティティの個別ファイル (.entity) を物理ディスクに保存します。
 * @param _filename シーンファイル名
 * @param _ecsGroup 対象のECSグループ
 */
void SceneIO::SaveScene(const std::string& _filename, ECSGroup* _ecsGroup) {
	nlohmann::json sceneJson = nlohmann::json::object();
	std::string sceneName = FileSystem::FileNameWithoutExtension(_filename);
	std::string sceneDir = fileDirectory_ + sceneName + "/";
	std::filesystem::create_directories(sceneDir);

	// 現在のシーンに含まれるエンティティのファイル名リスト
	std::unordered_set<std::string> currentEntityFiles;

	auto& entities = _ecsGroup->GetEntities();
	for (auto& entity : entities) {
		if (entity->GetId() < 0) continue;

		if (Variables* var = entity->GetComponent<Variables>()) {
			var->ReloadScriptVariables();
		}

		nlohmann::json entityJson = EntityJsonConverter::ToJson(entity.get());
		if (entityJson.empty()) continue;

		// 名前衝突を避けるため、ファイル名にはGUIDを使用する
		std::string entityFileName = entity->GetGuid().ToString() + ".entity";
		std::string entityPath = sceneDir + entityFileName;
		
		currentEntityFiles.insert(entityFileName);

		// .entityファイルを保存
		std::ofstream ofs(entityPath);
		if (ofs) {
			ofs << entityJson.dump(4);
			ofs.close();
		}

		// シーンファイルには参照を保存
		nlohmann::json reference;
		reference["path"] = "./" + sceneName + "/" + entityFileName;
		reference["guid"] = entity->GetGuid().ToString();
		if (entity->GetParent()) {
			reference["parentGuid"] = entity->GetParent()->GetGuid().ToString();
		} else {
			reference["parentGuid"] = nullptr;
		}
		sceneJson["entities"].push_back(reference);
	}

	// 不要になった（削除された）エンティティファイルを物理削除
	if (std::filesystem::exists(sceneDir)) {
		for (const auto& entry : std::filesystem::directory_iterator(sceneDir)) {
			if (entry.is_regular_file()) {
				std::string fileName = entry.path().filename().string();
				// .entity 拡張子で、かつ現在のエンティティリストに含まれていないファイルを削除
				if (fileName.ends_with(".entity") && currentEntityFiles.find(fileName) == currentEntityFiles.end()) {
					std::filesystem::remove(entry.path());
					Console::Log("SceneIO: Deleted orphaned entity file: " + fileName);
				}
			}
		}
	}

	OutputJson(sceneJson, _filename);
}

/**
 * @brief シーンファイルを読み込み、そこに記述された各エンティティの定義ファイルを読み込みロードします。
 * @param _filename シーンファイル名
 * @param _ecsGroup 対象のECSグループ
 */
void SceneIO::LoadScene(const std::string& _filename, ECSGroup* _ecsGroup) {
	MonoScriptEngine::GetInstance().ClearECSGroup(_ecsGroup->GetGroupName());

	std::ifstream inputFile(fileDirectory_ + _filename);
	if (!inputFile.is_open()) {
		Console::Log("SceneIO: ファイルのオープンに失敗しました: " + fileDirectory_ + _filename);
		return;
	}

	nlohmann::json sceneJson;
	inputFile >> sceneJson;
	inputFile.close();

	if (!sceneJson.contains("entities")) return;

	nlohmann::json fullSceneJson = nlohmann::json::object();
	std::string sceneDirName = FileSystem::FileNameWithoutExtension(_filename);

	for (const auto& entityRef : sceneJson["entities"]) {
		if (entityRef.contains("path")) {
			std::string entityPath = entityRef["path"];
			
			// 1. 指定されたパスをまず試す
			std::ifstream entityFile(fileDirectory_ + entityPath);
			
			// 2. 失敗した場合、新旧両方の命名規則で再試行
			if (!entityFile.is_open()) {
				// GUIDベースのパスを作成して試す
				if (entityRef.contains("guid")) {
					std::string guidPath = "./" + sceneDirName + "/" + entityRef["guid"].get<std::string>() + ".entity";
					entityFile.open(fileDirectory_ + guidPath);
				}
				
				// まだ開かない場合、互換性のためエンティティ名ベースで試す (旧仕様)
				if (!entityFile.is_open() && entityRef.contains("name")) {
					std::string namePath = "./" + sceneDirName + "/" + entityRef["name"].get<std::string>() + ".entity";
					entityFile.open(fileDirectory_ + namePath);
				}
			}

			if (entityFile.is_open()) {
				nlohmann::json entityJson;
				entityFile >> entityJson;
				entityFile.close();

				// シーンファイル側の情報を優先（親子関係など）
				if (entityRef.contains("guid")) entityJson["guid"] = entityRef["guid"];
				if (entityRef.contains("parentGuid")) entityJson["parentGuid"] = entityRef["parentGuid"];
				
				// 互換性維持: 旧フォーマットの読み込み
				if (entityRef.contains("id") && !entityJson.contains("id")) entityJson["id"] = entityRef["id"];
				if (entityRef.contains("parent") && !entityJson.contains("parentGuid")) entityJson["parent"] = entityRef["parent"];

				fullSceneJson["entities"].push_back(entityJson);
			} else {
				Console::LogError("SceneIO: Entityファイルの読み込みに失敗しました: " + entityPath);
			}
		} else {
			// 旧フォーマット（直接エンティティデータが入っている場合）への対応
			fullSceneJson["entities"].push_back(entityRef);
		}
	}

	LoadSceneFromJson(fullSceneJson, _ecsGroup);
}

/**
 * @brief ECSグループ内の全エンティティを走査し、シリアライズ用のJSON形式に変換します。
 * @param _output 出力先JSON
 * @param _ecsGroup 対象のECSグループ
 */
void SceneIO::SaveSceneToJson(nlohmann::json& _output, ECSGroup* _ecsGroup) {

	auto& entities = _ecsGroup->GetEntities();
	for (auto& entity : entities) {
		/// マイナスIDはruntimeに生成されたエンティティなのでスキップ
		if (entity->GetId() < 0) {
			continue;
		}

		if (Variables* var = entity->GetComponent<Variables>()) {
			var->ReloadScriptVariables();
		}

		nlohmann::json entityJson = EntityJsonConverter::ToJson(entity.get());
		if (entityJson.empty()) {
			continue; // エンティティの情報が空ならスキップ
		}

		_output["entities"].push_back(entityJson);
	}

}

/**
 * @brief シーンのJSONデータから、エンティティおよびコンポーネントを生成・復元し、親子関係を設定します。
 * @param _input 入力JSON
 * @param _ecsGroup ロード先のECSグループ
 */
void SceneIO::LoadSceneFromJson(const nlohmann::json& _input, ECSGroup* _ecsGroup) {
	std::unordered_map<Guid, GameEntity*> entityMap;
	std::unordered_map<uint32_t, GameEntity*> oldIdMap; // 互換性用

	if (!_input.contains("entities")) {
		return;
	}

	/// 実際にシーンに変換する
	for (const auto& entityJson : _input["entities"]) {
		const std::string& prefabName = entityJson.value("prefabName", "");
		const std::string& entityName = entityJson.value("name", "");
		
		/// guidの取得、無効値なら新規生成
		Guid guid = Guid::kInvalid;
		if (entityJson.contains("guid")) {
			guid = Guid::FromString(entityJson["guid"].get<std::string>());
		}

		if (guid == Guid::kInvalid) {
			guid = GenerateGuid();
		}

		GameEntity* entity = _ecsGroup->GenerateEntity(guid, false);
		if (!prefabName.empty()) {
			_ecsGroup->GetEntityCollection()->ApplyPrefabToEntity(entity, prefabName);
		}

		if (entity) {
			entity->prefabName_ = prefabName;
			entity->name_ = entityName;

			/// シーンに保存されたjsonからエンティティを復元
			EntityJsonConverter::FromJson(entityJson, entity, _ecsGroup->GetGroupName());

			entityMap[guid] = entity;
			if (entityJson.contains("id")) {
				oldIdMap[entityJson["id"].get<uint32_t>()] = entity;
			}
		}
	}


	/// エンティティの親子関係を設定
	for (const auto& entityJson : _input["entities"]) {
		GameEntity* entity = nullptr;
		if (entityJson.contains("guid")) {
			entity = entityMap[Guid::FromString(entityJson["guid"])];
		} else if (entityJson.contains("id")) {
			entity = oldIdMap[entityJson["id"]];
		}
		
		if (!entity) continue;

		// 新フォーマット (parentGuid) を優先
		if (entityJson.contains("parentGuid") && !entityJson["parentGuid"].is_null()) {
			Guid parentGuid = Guid::FromString(entityJson["parentGuid"]);
			if (entityMap.contains(parentGuid)) {
				entity->SetParent(entityMap[parentGuid]);
			}
		}
		// 旧フォーマット (parent ID) もサポート
		else if (entityJson.contains("parent") && !entityJson["parent"].is_null()) {
			uint32_t parentId = entityJson["parent"];
			if (oldIdMap.contains(parentId)) {
				entity->SetParent(oldIdMap[parentId]);
			}
		}
	}

	// C++でロードしたデータをC#側に同期する
	MonoScriptEngine::GetInstance().SyncInitialComponentsToCS(_ecsGroup);
}

/**
 * @brief JSONデータを指定したファイル名でディスクに書き出します。
 * @param _json 保存対象のJSONデータ
 * @param _filename 保存先ファイル名
 */
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
