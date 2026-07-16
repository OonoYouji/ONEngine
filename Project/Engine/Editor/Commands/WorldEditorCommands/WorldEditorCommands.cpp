#include "WorldEditorCommands.h"

/// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <algorithm>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/ECS/Entity/Collection/EntityCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"

/// editor
#include "Engine/Editor/Clipboard/Clipboard.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Math/ImGuiMath.h"


using namespace Editor;

/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
CreateGameObjectCommand::CreateGameObjectCommand(ONEngine::ECSGroup* _ecs, const std::string& _name, ONEngine::GameEntity* _parentEntity)
	: entityName_(_name) {
	pEcsGroup_ = _ecs;
	parentGuid_ = ONEngine::Guid::kInvalid;
	if (_parentEntity) {
		parentGuid_ = _parentEntity->GetGuid();
	}
}

/**
 * @brief デストラクタ
 */
CreateGameObjectCommand::~CreateGameObjectCommand() {}

/**
 * @brief コマンドの実行（オブジェクトを生成し、親子関係や名前を割り当てます）。
 */
EDITOR_STATE CreateGameObjectCommand::Execute() {

	/// 生成するEntityのGuidを生成
	if (!generatedGuid_.CheckValid()) {
		generatedGuid_ = ONEngine::GenerateGuid();
	}

	generatedEntity_ = pEcsGroup_->GenerateEntity(generatedGuid_, false);

	EDITOR_STATE state = EDITOR_STATE_RUNNING;
	if (generatedEntity_) {
		generatedEntity_->SetName(entityName_);

		state = EDITOR_STATE_FINISH;

		/// 親子関係の設定
		if (parentGuid_.CheckValid()) {
			ONEngine::GameEntity* entity = pEcsGroup_->GetEntityFromGuid(parentGuid_);
			if (entity) {
				generatedEntity_->SetParent(entity);
			}
		}
	}

	return state;
}

/**
 * @brief コマンドの取り消し処理（生成したオブジェクトをECSグループから破棄します）。
 */
EDITOR_STATE CreateGameObjectCommand::Undo() {
	if (parentGuid_.CheckValid()) {
		ONEngine::GameEntity* parentEntity = pEcsGroup_->GetEntityFromGuid(parentGuid_);
		if (parentEntity && generatedEntity_) {
			generatedEntity_->SetParent(nullptr);
		}
	}


	pEcsGroup_->RemoveEntity(generatedEntity_);

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// プリミティブなオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
CreatePrimitiveCommand::CreatePrimitiveCommand(ONEngine::ECSGroup* _ecs, Type _type, ONEngine::GameEntity* _parentEntity)
	: type_(_type) {
	pEcsGroup_ = _ecs;
	parentGuid_ = ONEngine::Guid::kInvalid;
	if (_parentEntity) {
		parentGuid_ = _parentEntity->GetGuid();
	}
}

/**
 * @brief コマンドの実行（指定されたコンポーネントが付随したプリミティブエンティティを生成します）。
 */
EDITOR_STATE CreatePrimitiveCommand::Execute() {
	if (!generatedGuid_.CheckValid()) {
		generatedGuid_ = ONEngine::GenerateGuid();
	}

	generatedEntity_ = pEcsGroup_->GenerateEntity(generatedGuid_, false);
	if (!generatedEntity_) return EDITOR_STATE_FAILED;

	std::string baseName;
	switch (type_) {
	case Type::Camera:
		baseName = "Camera";
		generatedEntity_->AddComponent("CameraComponent");
		break;
	case Type::DirectionalLight:
		baseName = "DirectionalLight";
		generatedEntity_->AddComponent("DirectionalLight");
		break;
	case Type::Mesh:
		baseName = "Mesh";
		generatedEntity_->AddComponent("MeshRenderer");
		break;
	}

	uint32_t count = pEcsGroup_->CountEntity(baseName);
	std::string newName = baseName;
	if (count > 0) {
		newName += "_" + std::to_string(count);
	}
	generatedEntity_->SetName(newName);

	if (parentGuid_.CheckValid()) {
		ONEngine::GameEntity* parent = pEcsGroup_->GetEntityFromGuid(parentGuid_);
		if (parent) {
			generatedEntity_->SetParent(parent);
		}
	}

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（生成したプリミティブオブジェクトをECSグループから破棄します）。
 */
EDITOR_STATE CreatePrimitiveCommand::Undo() {
	if (generatedEntity_) {
		pEcsGroup_->RemoveEntity(generatedEntity_);
		generatedEntity_ = nullptr;
	}
	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// オブジェクトの名前変更コマンド
/// ///////////////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
EntityRenameCommand::EntityRenameCommand(ONEngine::GameEntity* _entity, const std::string& _newName)
	: pEntity_(_entity) {
	oldName_ = pEntity_->GetName();
	newName_ = _newName;
}

/**
 * @brief コマンドの実行（新しい名前を設定します）。
 */
EDITOR_STATE EntityRenameCommand::Execute() {

	if (pEntity_ == nullptr) {
		ONEngine::Console::Log("EntityRenameCommand : Entity is nullptr");
		return EDITOR_STATE_RUNNING;
	}

	pEntity_->SetName(newName_);

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（元の古い名前に戻します）。
 */
EDITOR_STATE EntityRenameCommand::Undo() {

	if (pEntity_) {
		pEntity_->SetName(oldName_);
	} else {
		ONEngine::Console::Log("EntityRenameCommand : Entity is nullptr");
	}

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
CreateNewEntityClassCommand::CreateNewEntityClassCommand(ONEngine::GameEntity* _entity, const std::string& _outputFilePath)
	: pEntity_(_entity) {
	pEntity_ = _entity;
	sourceClassPath_ = "Engine/Editor/Commands/WorldEditorCommands/SourceEntity";
	sourceClassName_ = "SourceEntity";
	outputFilePath_ = _outputFilePath;
}

/**
 * @brief コマンドの実行（新規C#クラスファイルを生成します）。
 */
EDITOR_STATE CreateNewEntityClassCommand::Execute() {
	if (pEntity_->GetName().empty()) {
		ONEngine::Console::LogError("CreateNewEntityClassCommand : Entity name is empty");
		return EDITOR_STATE_FAILED;
	}

	CreateNewClassFile(sourceClassPath_ + ".h", outputFilePath_, pEntity_->GetName() + ".h");
	CreateNewClassFile(sourceClassPath_ + ".cpp", outputFilePath_, pEntity_->GetName() + ".cpp");

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理。
 */
EDITOR_STATE CreateNewEntityClassCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/**
 * @brief テンプレートソースファイルをベースに、指定の名前空間やクラス名に置き換えて新しいスクリプトファイルを書き出します。
 */
EDITOR_STATE CreateNewEntityClassCommand::CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName) {

	// ファイルを読み込む
	std::ifstream inputFile(_srcFilePath);
	if (!inputFile) {
		ONEngine::Console::LogError("ファイルを開けません: " + _srcFilePath);
		return EDITOR_STATE_FAILED;
	}

	std::stringstream buffer;
	buffer << inputFile.rdbuf(); // ファイル全体を読み込む
	std::string content = buffer.str();

	inputFile.close();

	// 置き換える
	ONEngine::FileSystem::ReplaceAll(&content, sourceClassName_, pEntity_->GetName());

	// 新しいファイルに書き込む
	std::ofstream outputFile(_outputFileName + "/" + _newClassName);
	if (!outputFile) {
		ONEngine::Console::LogError("ファイルを書き込めません: " + _outputFileName);
		return EDITOR_STATE_FAILED;
	}

	outputFile << content;
	outputFile.close();

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
DeleteEntityCommand::DeleteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _entity)
	: pEcsGroup_(_ecs), pEntity_(_entity) {
}

/**
 * @brief コマンドの実行（エンティティをECSグループから破棄します）。
 */
EDITOR_STATE DeleteEntityCommand::Execute() {
	if (!pEcsGroup_ || !pEntity_) {
		ONEngine::Console::LogError("DeleteEntityCommand : ECS or Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	pEcsGroup_->RemoveEntity(pEntity_);

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（削除されたエンティティ階層とコンポーネントを完全に再構築します）。
 */
EDITOR_STATE DeleteEntityCommand::Undo() {
	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// プレハブを作成するコマンド
/// ///////////////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
CreatePrefabCommand::CreatePrefabCommand(ONEngine::GameEntity* _entity)
	: pEntity_(_entity) {
	if (pEntity_ == nullptr) {
		ONEngine::Console::LogError("CreatePrefabCommand : Entity is nullptr");
		return;
	}

	/// プレハブのパスを設定
	prefabName_ = pEntity_->GetName() + ".prefab";
}

/**
 * @brief コマンドの実行（プレハブファイルを出力します）。
 */
EDITOR_STATE CreatePrefabCommand::Execute() {

	/// ディレクトリがあるのかチェック
	if (!std::filesystem::exists(prefabPath_)) {
		std::filesystem::create_directories(prefabPath_);
	}

	/// スクリプトの変数を最新の状態にする
	if (ONEngine::Variables* var = pEntity_->GetComponent<ONEngine::Variables>()) {
		var->ReloadScriptVariables();
	}


	/// jsonに変換
	nlohmann::json entityJson = ONEngine::EntityJsonConverter::ToJson(pEntity_, true);

	/// 子の要素も入れる
	SerializeRecursive(pEntity_, entityJson);


	/// jsonが空ならログを残して終了
	if (entityJson.empty()) {
		ONEngine::Console::LogError("CreatePrefabCommand : EntityJson is empty");
		return EDITOR_STATE_FAILED;
	}


	/// ファイルに出力
	std::string prefabPath = "./Assets/Prefabs/" + prefabName_;
	std::ofstream outputFile(prefabPath);
	if (!outputFile.is_open()) {
		ONEngine::Console::LogError("CreatePrefabCommand : Failed to open prefab file: " + prefabPath);
		return EDITOR_STATE_FAILED;
	}

	outputFile << entityJson.dump(4);
	outputFile.close();
	ONEngine::Console::Log("Prefab created: " + prefabPath);

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理。
 */
EDITOR_STATE CreatePrefabCommand::Undo() {
	return EDITOR_STATE_FINISH;
}

/**
 * @brief 再帰的にエンティティ階層および各コンポーネントのシリアライズ情報を構築します。
 */
void CreatePrefabCommand::SerializeRecursive(ONEngine::GameEntity* _entity, nlohmann::json& _json) {
	/// 子の要素も入れる
	for (auto& child : _entity->GetChildren()) {
		/// クローンオブジェクトはスキップ
		if (child->GetId() < 0) {
			continue;
		}

		nlohmann::json childJson = ONEngine::EntityJsonConverter::ToJson(child, true);
		SerializeRecursive(child, childJson);
		_json["children"].push_back(childJson);
	}
}


/// ///////////////////////////////////////////////////
/// エンティティをコピーするコマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
CopyEntityCommand::CopyEntityCommand(ONEngine::GameEntity* _entity) : pEntity_(_entity) {}

/**
 * @brief コマンドの実行（クリップボード用のシリアライズ処理を行います）。
 */
EDITOR_STATE CopyEntityCommand::Execute() {
	/// jsonに変換
	entityJson_ = ONEngine::EntityJsonConverter::ToJson(pEntity_, true);
	EditCommand::SetClipboardData(entityJson_);

	/// チェック
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (copiedEntity) {
		/// stringに変換してログに出す
		std::string jsonString = copiedEntity->dump(4);
		ONEngine::Console::Log("Copied Entity JSON:\n" + jsonString);
	}


	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理。
 */
EDITOR_STATE CopyEntityCommand::Undo() {
	/// 特にやることなし
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/**
 * @brief コンストラクタ
 */
PasteEntityCommand::PasteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _selectedEntity)
	: pEcsGroup_(_ecs), pSelectedEntity_(_selectedEntity) {
}

/**
 * @brief コマンドの実行（JSONをデシリアライズして新規エンティティとしてインスタンス化します）。
 */
EDITOR_STATE PasteEntityCommand::Execute() {
	/// クリップボードからデータを取得
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (!copiedEntity || copiedEntity->empty()) {
		ONEngine::Console::LogError("PasteEntityCommand : Clipboard is empty or invalid");
		return EDITOR_STATE_FAILED;
	}

	/// jsonからエンティティを生成
	std::string originalName = (*copiedEntity)["name"].get<std::string>();

	pastedEntity_ = pEcsGroup_->GenerateEntity(ONEngine::GenerateGuid(), ONEngine::DebugConfig::isDebugging);
	ONEngine::EntityJsonConverter::FromJson(*copiedEntity, pastedEntity_, pEcsGroup_->GetGroupName());
	if (!pastedEntity_) {
		ONEngine::Console::LogError("PasteEntityCommand : Failed to create entity from JSON");
		return EDITOR_STATE_FAILED;
	}

	/// 新しい名前を設定 (名前被りを回避)
	std::string baseName = originalName;
	std::string newName = baseName;
	int suffixCount = 1;

	auto nameExists = [&](const std::string& name) {
		for (auto& e : pEcsGroup_->GetEntities()) {
			if (e->GetName() == name) return true;
		}
		return false;
	};

	while (nameExists(newName)) {
		newName = std::format("{}_{}", baseName, suffixCount++);
	}

	pastedEntity_->SetName(newName);
	if (pSelectedEntity_) {
		pastedEntity_->SetParent(pSelectedEntity_);
	}

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（ペーストしたオブジェクトをシーンから破棄します）。
 */
EDITOR_STATE PasteEntityCommand::Undo() {
	if (pastedEntity_) {
		pEcsGroup_->RemoveEntity(pastedEntity_);
		pastedEntity_ = nullptr;
		return EDITOR_STATE_FINISH;
	}
	return EDITOR_STATE_FAILED;
}


/// ///////////////////////////////////////////////////
/// エンティティの親子関係を変更するコマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
ChangeEntityParentCommand::ChangeEntityParentCommand(ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent)
	: pEntity_(_entity), pNewParent_(_newParent) {
}

/**
 * @brief コマンドの実行（親ポインタを変更し、相対位置などを再計算して同期します）。
 */
EDITOR_STATE ChangeEntityParentCommand::Execute() {
	if (!pEntity_) {
		ONEngine::Console::LogError("ChangeEntityParentCommand : Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}
	/// 古い親を保存
	pOldParent_ = pEntity_->GetParent();
	/// 親を変更
	pEntity_->SetParent(pNewParent_);
	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（元の古い親オブジェクトへ戻します）。
 */
EDITOR_STATE ChangeEntityParentCommand::Undo() {
	if (!pEntity_) {
		ONEngine::Console::LogError("ChangeEntityParentCommand : Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	pEntity_->RemoveParent();

	/// 親を元に戻す
	if (pOldParent_) {
		pEntity_->SetParent(pOldParent_);
	}
	return EDITOR_STATE_FINISH;
}

/// ///////////////////////////////////////////////////
/// エンティティの順番を入れ替えるコマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
ReorderEntityCommand::ReorderEntityCommand(ONEngine::ECSGroup* _ecsGroup, ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent, uint32_t _newIndex)
	: pEcsGroup_(_ecsGroup), pEntity_(_entity), pNewParent_(_newParent), newIndex_(_newIndex) {
	pOldParent_ = pEntity_->GetParent();

	// 古いインデックスを保存
	if (pOldParent_) {
		const auto& children = pOldParent_->GetChildren();
		auto it = std::find(children.begin(), children.end(), pEntity_);
		oldIndex_ = static_cast<uint32_t>(std::distance(children.begin(), it));
	} else {
		const auto& entities = pEcsGroup_->GetEntities();
		auto it = std::find_if(entities.begin(), entities.end(), [this](const std::unique_ptr<ONEngine::GameEntity>& e) {
			return e.get() == pEntity_;
		});
		oldIndex_ = static_cast<uint32_t>(std::distance(entities.begin(), it));
	}
}

/**
 * @brief コマンドの実行（指定のインデックス位置にエンティティを並び替えます）。
 */
EDITOR_STATE ReorderEntityCommand::Execute() {
	if (!pEntity_) return EDITOR_STATE_FAILED;

	pEntity_->SetParent(pNewParent_);

	if (pNewParent_) {
		pNewParent_->MoveChild(pEntity_, newIndex_);
	} else {
		pEcsGroup_->GetEntityCollection()->MoveEntity(pEntity_, newIndex_);
	}

	return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（並び替え前のインデックスに戻します）。
 */
EDITOR_STATE ReorderEntityCommand::Undo() {
	if (!pEntity_) return EDITOR_STATE_FAILED;

	pEntity_->SetParent(pOldParent_);

	if (pOldParent_) {
		pOldParent_->MoveChild(pEntity_, oldIndex_);
	} else {
		pEcsGroup_->GetEntityCollection()->MoveEntity(pEntity_, oldIndex_);
	}

	return EDITOR_STATE_FINISH;
}

/// ///////////////////////////////////////////////////
/// プレハブからインスタンスを作成するコマンド
/// ///////////////////////////////////////////////////

/**
 * @brief コンストラクタ
 */
InstantiatePrefabCommand::InstantiatePrefabCommand(ONEngine::ECSGroup* _ecs, const std::string& _prefabPath, ONEngine::GameEntity* _parentEntity)
	: prefabPath_(_prefabPath) {
	pEcsGroup_ = _ecs;
	parentGuid_ = ONEngine::Guid::kInvalid;
	if (_parentEntity) {
		parentGuid_ = _parentEntity->GetGuid();
	}
}

/**
 * @brief コマンドの実行（プレハブ情報をデシリアライズして生成します）。
 */
EDITOR_STATE InstantiatePrefabCommand::Execute() {
	// ファイル名（パスから拡張子を除いたもの）を抽出
	std::filesystem::path path(prefabPath_);
	std::string prefabName = path.filename().string();

	// エディタ上での生成なので _isRuntime = false
	generatedEntity_ = pEcsGroup_->GenerateEntityFromPrefab(prefabName, false);

	if (generatedEntity_) {
		generatedGuid_ = generatedEntity_->GetGuid();

		if (parentGuid_.CheckValid()) {
			ONEngine::GameEntity* parent = pEcsGroup_->GetEntityFromGuid(parentGuid_);
			if (parent) {
				generatedEntity_->SetParent(parent);
			}
		}
		return EDITOR_STATE_FINISH;
	}

	return EDITOR_STATE_FAILED;
}

/**
 * @brief コマンドの取り消し処理（生成されたオブジェクト階層を削除します）。
 */
EDITOR_STATE InstantiatePrefabCommand::Undo() {
	if (generatedEntity_) {
		pEcsGroup_->RemoveEntity(generatedEntity_);
		generatedEntity_ = nullptr;
	}
	return EDITOR_STATE_FINISH;
}
