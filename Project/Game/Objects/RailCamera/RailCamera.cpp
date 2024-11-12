#define NOMINMAX
#include "RailCamera.h"

/// std
#include <cassert>
#include <algorithm>

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"
#include "Math/Random.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"


RailCamera::RailCamera() {
	CreateTag(this);
}

RailCamera::~RailCamera() {}



void RailCamera::Initialize() {

	/// transform
	pTransform_->rotateOrder = QUATERNION;

	//moveSpeed_ = 0.0f;
	moveSpeed_ = 0.25f;

	/// move param setting...
	Reset();

}

void RailCamera::Update() {

	Move();
	
	/// 進行方向にライトを向ける
	pDirectionalLight_->SetDirection(moveDirection_.Normalize());



	/// 最後のアンカーまでたどり着いたか確認
	if(static_cast<int>(movingTime_) == static_cast<int>(anchorPointArraySize_ - 1)) {
		isMoveEnd_ = true;
	}

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

void RailCamera::Move() {
	/// shooting courseからanchor point arrayをもらう
	const std::vector<AnchorPoint>& anchorPointArray = pShootingCourse_->GetAnchorPointArray();
	anchorPointArraySize_ = anchorPointArray.size();
	
	if(moveSpeedRates_.empty()) {
		moveSpeedRates_.resize(anchorPointArraySize_);
	
		for(size_t i = 0; i < moveSpeedRates_.size(); ++i) {
			float& rate = moveSpeedRates_[i];
			rate = 1.0f * Random::Float(0.8f, 1.5f);

			if(i > moveSpeedRates_.size() - 7) {
				rate = 3.0f;
			}

		}
	}

	/// 前フレームのデータ保存
	preMovingTime_ = movingTime_;

	/// 移動 and 回転
	movingTime_ += moveSpeed_ * moveSpeedRates_[static_cast<size_t>(movingTime_)] * Time::DeltaTime();
	movingTime_ = std::clamp(movingTime_, 0.0f, anchorPointArraySize_ + 1.0f);

	nextMoveT_ = 1.0f / anchorPointArraySize_ * movingTime_;
	futureMoveT_ = 1.0f / anchorPointArraySize_ * (movingTime_ + futureTime_);


	/// anchor pointの更新
	nextAnchor_ = SplinePosition(anchorPointArray, nextMoveT_);
	futureAnchor_ = SplinePosition(anchorPointArray, futureMoveT_);

	/// 座標更新
	moveDirection_ = Vec3::Normalize(nextAnchor_.position - pTransform_->position);
	pTransform_->position = nextAnchor_.position;

	upDirection_ = nextAnchor_.up.Normalize();
	rightDirection_ = Vec3::Cross(upDirection_, moveDirection_).Normalize();

	//// オブジェクトが向くべき進行方向ベクトルからquaternion計算
	pTransform_->quaternion = Quaternion::LockAt({}, moveDirection_);

}



void RailCamera::SetShootingCourse(ShootingCourse* _shootingCourse) {
	pShootingCourse_ = _shootingCourse;
}

void RailCamera::SetDirectionalLight(DirectionalLight* _directionalLight) {
	pDirectionalLight_ = _directionalLight;
}

void RailCamera::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	assert(pGameCamera_);

	/// thisがgame cameraの親
	pGameCamera_->SetParent(pTransform_);
}
