#include "ComponentEditCommands.h"

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
}

EDITOR_STATE EntityDataOutputCommand::Execute() {

	nlohmann::json jsonData;

	for (auto& component : pEntity_->GetComponents()) {
		jsonData.push_back(ComponentJsonConverter::ToJson(component.second));
	}

	return EDITOR_STATE();
}

EDITOR_STATE EntityDataOutputCommand::Undo() {
	return EDITOR_STATE();
}

