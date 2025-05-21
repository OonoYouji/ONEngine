#include "WorldEditorCommands.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EmptyEntity/EmptyEntity.h"

/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

CreateGameObjectCommand::CreateGameObjectCommand(EntityComponentSystem* _ecs) {
	pECS_ = _ecs;
}

CreateGameObjectCommand::~CreateGameObjectCommand() {}

EDITOR_STATE CreateGameObjectCommand::Execute() {
	generatedEntity_ = pECS_->GenerateEntity<EmptyEntity>();

	EDITOR_STATE state = EDITOR_STATE_RUNNING;
	if (generatedEntity_) {
		state = EDITOR_STATE_FINISH;
	}

	return state;
}

EDITOR_STATE CreateGameObjectCommand::Undo() {
	pECS_->RemoveEntity(generatedEntity_);

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// オブジェクトの名前変更コマンド
/// ///////////////////////////////////////////////////
EntityRenameCommand::EntityRenameCommand(IEntity* _entity)
	: pEntity_(_entity) {}

EDITOR_STATE EntityRenameCommand::Execute() {
	EDITOR_STATE result = EDITOR_STATE_RUNNING;

	if (pEntity_) {

		// 変更前の名前を保存
		oldName_ = pEntity_->GetName();
		newName_ = oldName_;
		newName_ += "_Renamed";
		pEntity_->SetName(newName_);

		//if (Input::TriggerKey(DIK_RETURN)) {
			result = EDITOR_STATE_FINISH;
		//}


	} else if (pEntity_ == nullptr) {
		result = EDITOR_STATE_FAILED;
		Console::Log("EntityRenameCommand : Entity is nullptr");


	} else {
		result = EDITOR_STATE_FAILED;
	}

	return result;
}

EDITOR_STATE EntityRenameCommand::Undo() {

	if (pEntity_) {
		pEntity_->SetName(oldName_);
	} else {
		Console::Log("EntityRenameCommand : Entity is nullptr");
	}

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


