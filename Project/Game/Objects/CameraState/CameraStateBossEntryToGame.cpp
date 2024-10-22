#define NOMINMAX
#include "CameraStateBossEntryToGame.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"

/// objects
#include "Objects/Camera/GameCamera.h"



void CameraStateBossEntryToGame::Initialize() {

	moveTime_    = 0.0f;
	maxMoveTime_ = 1.5f;

	startData_ = { { -4.2f, 3.0f + 11.0f, -9.15f }, { 0.3f, 0.566f, 0.0f } };
	endData_   = { { 0.0f, 0.0f, -40.0f }, { 0.0f, 0.0f, 0.0f } };

}

void CameraStateBossEntryToGame::Update() {
	assert(pGameCamera_);

	moveTime_ += Time::TimeRateDeltaTime();
	float lerpT = std::min(moveTime_ / maxMoveTime_, 1.0f);

	Vec3 position = Vec3::Lerp(
		startData_.position, endData_.position, 
		lerpT
	);
	
	Vec3 rotate = Vec3::Lerp(
		startData_.rotate, endData_.rotate,
		lerpT
	);


	pGameCamera_->SetPosition(position);
	pGameCamera_->SetRotate(rotate);

}

void CameraStateBossEntryToGame::Debug() {
	if(ImGui::TreeNodeEx("debug")) {

		ImGui::SeparatorText("start data");
		ImGui::DragFloat3("start position", &startData_.position.x, 0.01f);
		ImGui::DragFloat3("start rotate",   &startData_.rotate.x,   0.01f);

		ImGui::SeparatorText("end data");
		ImGui::DragFloat3("end position",   &endData_.position.x,   0.01f);
		ImGui::DragFloat3("end rotate",     &endData_.rotate.x,     0.01f);

		ImGui::SeparatorText("movement time");
		ImGui::DragFloat("time",     &moveTime_,    0.01f, 0.0f, maxMoveTime_);
		ImGui::DragFloat("max tiem", &maxMoveTime_, 0.01f, 0.0f, 120.0f);

		ImGui::TreePop();
	}
}


void CameraStateBossEntryToGame::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
}

bool CameraStateBossEntryToGame::IsFinishedMoving() {
	if(moveTime_ / maxMoveTime_ >= 1.0f) {
		return true;
	}
	return false;
}
