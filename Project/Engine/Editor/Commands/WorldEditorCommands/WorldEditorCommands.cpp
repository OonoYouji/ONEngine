#include "WorldEditorCommands.h"

/// engine
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
	return EDITOR_STATE();
}

EDITOR_STATE EntityRenameCommand::Undo() {
	return EDITOR_STATE();
}
