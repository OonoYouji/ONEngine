#include "FocusEntityCommand.h"

namespace Editor {

FocusEntityCommand::FocusEntityCommand(ONEngine::EntityComponentSystem* ecs, ONEngine::GameEntity* targetEntity)
	: pEcs_(ecs), targetEntity_(targetEntity), isExecuted_(false) {}

EDITOR_STATE FocusEntityCommand::Execute() {
	if(!pEcs_ || !targetEntity_) {
		return EDITOR_STATE_FAILED; // ※エンジン側の定義に合わせてください
	}

	ONEngine::CameraComponent* editorCamera = pEcs_->GetECSGroup("Debug")->GetMainCamera();
	if(!editorCamera) {
		return EDITOR_STATE_FAILED;
	}

	ONEngine::Transform* transform = editorCamera->GetOwner()->GetComponent<ONEngine::Transform>();
	if(!transform) {
		return EDITOR_STATE_FAILED;
	}

	// 初回実行時のみ、元のカメラ位置・回転を保存しておく（Undo用）
	if(!isExecuted_) {
		previousPosition_ = transform->GetPosition();
		previousRotation_ = transform->GetRotate();
		isExecuted_ = true;
	}

	// --- 実際のフォーカス処理 ---
	ONEngine::Vector3 targetPos = targetEntity_->GetPosition();
	float distance = 10.0f;
	ONEngine::Vector3 cameraForward = { 0.0f, -0.5f, 1.0f };
	ONEngine::Vector3 newCameraPos;

	newCameraPos.x = targetPos.x - (cameraForward.x * distance);
	newCameraPos.y = targetPos.y - (cameraForward.y * distance);
	newCameraPos.z = targetPos.z - (cameraForward.z * distance);

	transform->SetPosition(newCameraPos);
	editorCamera->LookAt(targetPos - newCameraPos);
	editorCamera->UpdateViewProjection();

	return EDITOR_STATE_FINISH; // ※エンジン側の定義に合わせてください
}

EDITOR_STATE FocusEntityCommand::Undo() {
	if(!pEcs_) {
		return EDITOR_STATE_FAILED;
	}

	ONEngine::CameraComponent* editorCamera = pEcs_->GetECSGroup("Debug")->GetMainCamera();
	if(editorCamera) {
		ONEngine::Transform* transform = editorCamera->GetOwner()->GetComponent<ONEngine::Transform>();
		if(transform) {
			// カメラの座標・回転をフォーカス前の状態に戻す
			transform->SetPosition(previousPosition_);
			transform->SetRotate(previousRotation_);
			editorCamera->UpdateViewProjection();
		}
	}

	return EDITOR_STATE_FINISH;
}

} /// namespace Editor