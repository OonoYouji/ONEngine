#pragma once

#include "../IEditCommand.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Core/Utility/Math/Math.h"

namespace Editor {

class FocusEntityCommand : public IEditCommand {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	/// @brief コンストラクタ
	/// @param ecs ECSのポインタ（デバッグカメラの取得に必要）
	/// @param targetEntity フォーカス対象のエンティティ
	FocusEntityCommand(ONEngine::EntityComponentSystem* ecs, ONEngine::GameEntity* targetEntity);
	~FocusEntityCommand() override = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::GameEntity* targetEntity_;

	// Undo（取り消し）用に、フォーカス前のカメラの情報を保存しておく変数
	ONEngine::Vector3 previousPosition_;
	ONEngine::Quaternion previousRotation_;
	bool isExecuted_ = false;
};

} /// namespace Editor