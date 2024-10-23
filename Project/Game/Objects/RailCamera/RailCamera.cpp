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
	/// transform
	pTransform_->rotateOrder = XYZ;

	/// move param setting...
	Reset();

}

void RailCamera::Update() {

	/// shooting courseからanchor point arrayをもらう
	const std::vector<AnchorPoint>& anchorPointArray = pShootingCourse_->GetAnchorPointArray();
	const size_t kSegmentCount = anchorPointArray.size();

	/// 移動 and 回転
	movingTime_ += Time::DeltaTime();
	movingTime_ = std::clamp(movingTime_, 0.0f, kSegmentCount + 1.0f);
	nextMoveT_ = 1.0f / kSegmentCount * movingTime_;
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

	//pTransform_->rotate = {
	//	std::atan2(-moveDirection_.y, Vec3::Length({ moveDirection_.x, 0.0f, moveDirection_.z })),
	//	std::atan2(moveDirection_.x, moveDirection_.z),
	//	0.0f,
	//};
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
	upDirection_ = Vec3::kUp;
	movingTime_ = 0.0f;
	nextMoveT_ = 0.0f;
	futureMoveT_ = 0.0f;
	futureTime_ = 1.0f;
}



AnchorPoint RailCamera::SplinePosition(const std::vector<AnchorPoint>& anchorPointArray, float t) const {
	assert(anchorPointArray.size() >= 4 && "制御点は4点以上必要です");

	size_t division = anchorPointArray.size() - 1;
	float areaWidth = 1.0f / static_cast<float>(division);

	///- 区間内の始点を0.0f, 終点を1.0f としたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	///- 区画番号
	size_t index = static_cast<size_t>(t / areaWidth);
	index = std::min(index, anchorPointArray.size() - 1);

	size_t indices[4]{
		index - 1,
		index,
		index + 1,
		index + 2
	};

	if(index == 0) {
		indices[0] = indices[1];
	}

	if(indices[2] >= anchorPointArray.size()) {
		indices[2] = indices[1];
		indices[3] = indices[1];
	}

	if(indices[3] >= anchorPointArray.size()) {
		indices[3] = indices[2];
	}

	const AnchorPoint& p0 = anchorPointArray[indices[0]];
	const AnchorPoint& p1 = anchorPointArray[indices[1]];
	const AnchorPoint& p2 = anchorPointArray[indices[2]];
	const AnchorPoint& p3 = anchorPointArray[indices[3]];

	return SplineInterpolation({ p0, p1, p2, p3 }, t_2);
}

AnchorPoint RailCamera::SplineInterpolation(const std::array<AnchorPoint, 4>& anchorPointArray, float t) const {

	const float s = 0.5f;

	float t2 = t * t;  // t^2
	float t3 = t2 * t; // t^3

	/// positionの補完
	std::array<Vec3, 4> p{
		anchorPointArray[0].position,
		anchorPointArray[1].position,
		anchorPointArray[2].position,
		anchorPointArray[3].position
	};

	/// position element-> pe
	Vec3 pe3 = -p[0] + (p[1] * 3.0f) - (p[2] * 3.0f) + p[3];
	Vec3 pe2 = (p[0] * 2.0f) - (p[1] * 5.0f) + (p[2] * 4.0f) - p[3];
	Vec3 pe1 = -p[0] + p[2];
	Vec3 pe0 = p[1] * 2.0f;



	/// twistsの補完
	std::array<Vec3, 4> twists{
		anchorPointArray[0].up,
		anchorPointArray[1].up,
		anchorPointArray[2].up,
		anchorPointArray[3].up
	};

	/// twist element-> te
	Vec3 te3 = -twists[0] + (twists[1] * 3.0f) - (twists[2] * 3.0f) + twists[3];
	Vec3 te2 = (twists[0] * 2.0f) - (twists[1] * 5.0f) + (twists[2] * 4.0f) - twists[3];
	Vec3 te1 = -twists[0] + twists[2];
	Vec3 te0 = twists[1] * 2.0f;

	/// result
	return {
		((pe3 * t3) + (pe2 * t2) + (pe1 * t) + pe0) * s,
		((te3 * t3) + (te2 * t2) + (te1 * t) + te0) * s
	};
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
