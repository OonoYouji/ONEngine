#include "ComponentEditCommands.h"

/// std
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "ComponentJsonConverter.h"


/// ===============================================
/// Editor Command
/// ===============================================

EDITOR_STATE ComponentEditCommands::Execute() {
	// ここにコマンドの実行ロジックを実装
	// 例: コンポーネントの追加、削除、編集など
	return EDITOR_STATE_FINISH;
}

EDITOR_STATE ComponentEditCommands::Undo() {
	// ここにコマンドの取り消しロジックを実装
	// 例: コンポーネントの追加を取り消す、削除を取り消すなど
	return EDITOR_STATE_FINISH;
}


/// ////////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ////////////////////////////////////////////////

EntityDataOutputCommand::EntityDataOutputCommand(IEntity* _entity) {
	pEntity_ = _entity;
	outputFilePath_ = "Assets/Jsons/" + pEntity_->GetName() + "Components.json";
}

EDITOR_STATE EntityDataOutputCommand::Execute() {
	nlohmann::json jsonData;
	for (auto& component : pEntity_->GetComponents()) {
		jsonData.push_back(ComponentJsonConverter::ToJson(component.second));
	}

	std::filesystem::path path(outputFilePath_);
	std::filesystem::create_directories(path.parent_path());

	std::ofstream ofs(outputFilePath_);
	if (!ofs) {
		Console::Log("ファイルを開けませんでした: " + outputFilePath_);
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}

	ofs << jsonData.dump(4);

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE EntityDataOutputCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EntityDataInputCommand::EntityDataInputCommand(IEntity* _entity, const std::string& _filePath) {

}

EDITOR_STATE EntityDataInputCommand::Execute() {

	/// fileを開く
	std::ifstream ifs(inputFilePath_);
	if (!ifs) {
		Console::Log("ファイルを開けませんでした: " + inputFilePath_);
		return EDITOR_STATE();
	}


	/// jsonを読み込む
	nlohmann::json jsonData;
	ifs >> jsonData;

	/// コンポーネントを追加
	for (const auto& componentJson : jsonData) {
		const std::string componentType = componentJson.at("type").get<std::string>();
		pEntity_->AddComponent(componentType);
		//IComponent* component = ComponentJsonConverter::FromJson(componentJson);
	/*	if (component) {
			pEntity_->AddComponent(component);
		} else {
			Console::Log("コンポーネントの変換に失敗しました: " + componentJson.dump());
		}*/
	}

	return EDITOR_STATE();
}

EDITOR_STATE EntityDataInputCommand::Undo() {
	return EDITOR_STATE();
}
