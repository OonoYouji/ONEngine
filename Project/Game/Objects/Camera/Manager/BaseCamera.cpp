#define NOMINMAX
#include "BaseCamera.h"

#include <format>

#include "WindowManager/WinApp.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "Math/Easing.h"

namespace {

	float Cot(float t) {
		return 1.0f / std::tanf(t);
	}

	Mat4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		return Mat4(
			{ (1 / aspectRatio) * Cot(fovY / 2.0f), 0.0f,             0.0f,                                         0.0f },
			{ 0.0f,                                 Cot(fovY / 2.0f), 0.0f,                                         0.0f },
			{ 0.0f,                                 0.0f,             farClip / (farClip - nearClip),               1.0f },
			{ 0.0f,                                 0.0f,             (-nearClip * farClip) / (farClip - nearClip), 0.0f }
		);
	}

}


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseCamera::BaseCamera() {
	CreateTag(this);
}



void BaseCamera::Debug() {
	if(ImGui::TreeNodeEx("camera", ImGuiTreeNodeFlags_DefaultOpen)) {

		const char* labels[]{ "Perspective", "Orthographic" };
		ImGui::Combo("projection type", reinterpret_cast<int*>(&projectionType_), labels, 2);


		ImGui::TreePop();
	}


	/// 透視投影のデバッグ
	if(projectionType_ == PERSPECTIVE) {
		if(ImGui::TreeNodeEx("Perspective", ImGuiTreeNodeFlags_DefaultOpen)) {

			ImGui::DragFloat("fovY", &fovY_, 0.005f);

			ImGui::TreePop();
		}
	}

	/// 平行投影のデバッグ
	if(projectionType_ == ORTHOGRAPHIC) {
		if(ImGui::TreeNodeEx("Orthographic", ImGuiTreeNodeFlags_DefaultOpen)) {


			ImGui::TreePop();
		}
	}


}

/// ===================================================
/// 初期化
/// ===================================================
void BaseCamera::BaseInitialize() {

	UpdateMatView();
	UpdateMatPerspective();

	matVp_ = matView_ * matProjection_;

	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matVpData_));
	*matVpData_ = matVp_;

	Transfer();
}

void BaseCamera::BaseUpdate() {
	UpdateMatrix();
	UpdateMatView();
	UpdateMatPerspective();
	UpdateMatOrthographic();
	Transfer();
}



void BaseCamera::UpdateMatView() {
	matView_ = pTransform_->matTransform.Inverse();
}


void BaseCamera::UpdateMatPerspective() {
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, static_cast<float>(ONE::WinApp::kWindowSizeX) / static_cast<float>(ONE::WinApp::kWindowSizeY),
		nearZ_, farZ_);
}

void BaseCamera::UpdateMatOrthographic() {

	float distance    = pTransform_->position.Len();
	if(distance <= 0.0f) {
		return;
	}

	float aspectRatio = static_cast<float>(ONE::WinApp::kWindowSizeX) / static_cast<float>(ONE::WinApp::kWindowSizeY);
	float height      = 2.0f * distance * std::tan(fovY_ / 2.0f);
	float width       = height * aspectRatio;

	matOrtho_ = Mat4::MakeOrthographicMatrix(
		width, height, nearZ_, farZ_
	);

}


void BaseCamera::Move() {
	if(moveTime_ > maxMoveTime_) { return; }

	moveTime_ += Time::DeltaTime();

	float t = Ease::Out::Quart(std::min(moveTime_ / maxMoveTime_, 1.0f));
	Vec3 position = Vec3::Lerp(startMoveData_.position, endMoveData_.position, t);
	Vec3 rotate = Vec3::Lerp(startMoveData_.rotate, endMoveData_.rotate, t);
	SetPosition(position);
	SetRotate(rotate);
}


void BaseCamera::Transfer() {

	/// 透視投影か平行投影で matVp_ の計算を変更する
	if(projectionType_ == PERSPECTIVE) {
		matVp_ = matView_ * matProjection_;
	} else {
		matVp_ = matView_ * matOrtho_;
	}

	*matVpData_ = matVp_;
}

void BaseCamera::SetMove(const MoveData& start, const MoveData& end, float time) {
	moveTime_ = 0.0f;
	maxMoveTime_ = time;

	startMoveData_ = start;
	endMoveData_ = end;
}

void BaseCamera::SetProjectionType(PROJECTION_TYPE projectionType) {
	projectionType_ = projectionType;
}

