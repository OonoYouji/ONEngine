#define NOMINMAX
#include "RailCamera.h"

/// std
#include <cassert>
#include <algorithm>

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"


RailCamera::RailCamera() {
	CreateTag(this);
}

RailCamera::~RailCamera() {}



void RailCamera::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");
	meshRenderer->isActive = false;

	/// transform
	pTransform_->rotateOrder = YXZ;

	moveSpeed_ = 0.25f;

	/// move param setting...
	Reset();

}

void RailCamera::Update() {

	/// shooting courseからanchor point arrayをもらう
	const std::vector<AnchorPoint>& anchorPointArray = pShootingCourse_->GetAnchorPointArray();
	const size_t kSegmentCount = anchorPointArray.size();

	/// 前フレームのデータ保存
	preMovingTime_ = movingTime_;

	/// 移動 and 回転
	movingTime_ += moveSpeed_ * Time::DeltaTime();
	movingTime_ = std::clamp(movingTime_, 0.0f, kSegmentCount + 1.0f);

	nextMoveT_   = 1.0f / kSegmentCount * movingTime_;
	futureMoveT_ = 1.0f / kSegmentCount * (movingTime_ + futureTime_);


	/// anchor pointの更新
	nextAnchor_ = SplinePosition(anchorPointArray, nextMoveT_);
	futureAnchor_ = SplinePosition(anchorPointArray, futureMoveT_);

	/// 座標更新
	moveDirection_ = Vec3::Normalize(nextAnchor_.position - pTransform_->position);
	pTransform_->position = nextAnchor_.position;

	upDirection_ = nextAnchor_.up.Normalize();
	rightDirection_ = Vec3::Cross(upDirection_, moveDirection_).Normalize();

	// オブジェクトが向くべき進行方向ベクトルからオイラー角を計算
	pTransform_->rotate = {
		std::asin(-moveDirection_.y),
		std::atan2(moveDirection_.x, moveDirection_.z),
		std::atan2(rightDirection_.y, upDirection_.y),
	};

}

void RailCamera::Debug() {
	if(ImGui::TreeNodeEx("camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		if(ImGui::Button("reset")) {
			Reset();
		}
		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("move parameters", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("moving time", &movingTime_, 0.05f);
		ImGui::DragFloat("move speed", &moveSpeed_, 0.001f);

		ImGui::Spacing();

		ImGui::DragFloat("next moveT", &nextMoveT_, 0.05f);
		ImGui::DragFloat("future moveT", &futureMoveT_, 0.05f);

		ImGui::Spacing();

		ImGui::DragFloat("future time", &futureTime_, 0.05f);

		ImGui::Spacing();

		ImGui::DragFloat3("up diretion", &upDirection_.x, 0.01f);
		ImGui::DragFloat3("move diretion", &moveDirection_.x, 0.01f);
		ImGui::DragFloat3("right diretion", &rightDirection_.x, 0.01f);

		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("this param", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(size_t r = 0; r < 4; ++r) {
			for(size_t c = 0; c < 4; ++c) {
				if(c != 0) { ImGui::SameLine(); }
				ImGui::Text("%0.2f, ", pTransform_->matTransform.m[r][c]);
			}
		}

		ImGui::TreePop();
	}

	
	if(ImGui::TreeNodeEx("camera param", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(size_t r = 0; r < 4; ++r) {
			for(size_t c = 0; c < 4; ++c) {
				if(c != 0) { ImGui::SameLine(); }
				ImGui::Text("%0.2f, ", pGameCamera_->GetMatTransform().m[r][c]);
			}
		}

		ImGui::TreePop();
	}





}





void RailCamera::Reset() {
	/// move param setting...
	moveDirection_ = {};
	upDirection_   = Vec3::kUp;
	movingTime_    = 0.0f;
	nextMoveT_     = 0.0f;
	futureMoveT_   = 0.0f;
	futureTime_    = 1.0f;
}



void RailCamera::SetShootingCourse(ShootingCourse* _shootingCourse) {
	pShootingCourse_ = _shootingCourse;
}

void RailCamera::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	assert(pGameCamera_);

	/// thisがgame cameraの親
	pGameCamera_->SetParent(pTransform_);

}
