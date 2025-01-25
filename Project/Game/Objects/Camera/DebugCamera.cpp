#include "DebugCamera.h"

#include "Objects/Camera/Manager/CameraManager.h"
#include "Input/Input.h"

#include "ImGuiManager/ImGuiManager.h"


DebugCamera::DebugCamera() {
	SetName("DebugCamera");
	CameraManager::GetInstance()->AddCamera(GetName(), this);
}
DebugCamera::~DebugCamera() {}


/// ===================================================
/// 初期化
/// ===================================================
void DebugCamera::Initialize() {
	isActive = false;

	//pTransform_->position = { 0.0f, 0.0f, -6.49f * 2 };
	/*pTransform_->position = { 1.8f,0.87f, -12.7f };
	pTransform_->rotate = { 0.066f, -0.258f, 0.0f };*/

	BaseCamera::BaseInitialize();
}


/// ===================================================
/// 描画
/// ===================================================
void DebugCamera::Update() {
	if(!isActive) { return; }

	velocity_ = { 0.0f,0.0f,0.0f };

	if(Input::PressMouse(MouseCode::Center)) {
		velocity_ += (Vec3(Input::MouseVelocity(), 0.0f) / 256.0f) * -1.0f;
		velocity_.y *= -1.0f;
		velocity_.z = 0.0f;
	}

	velocity_.z = Input::MouseScroll() / 256.0f;
	velocity_.x *= 1.0f + pTransform_->position.Len() / 32.0f;
	velocity_.y *= 1.0f + pTransform_->position.Len() / 32.0f;

	if(Input::PressMouse(MouseCode::Right)) {
		pTransform_->rotate.x += Input::MouseVelocity().y / 256.0f;
		pTransform_->rotate.y += Input::MouseVelocity().x / 256.0f;
	}

	velocity_ = Mat4::Transform(velocity_, Mat4::MakeRotate(pTransform_->rotate));
	pTransform_->position += velocity_;

	matView_ = pTransform_->matTransform.Inverse();

	BaseUpdate();
}

void DebugCamera::Debug() {
	std::string&& text = "operation\n";
	text += "left ctrl + mouse wheel + mouse moving: move\n";
	text += "mouse wheel scroll: zoom in, zoom out\n";

	/// テキスト表示
	ImGui::InputTextMultiline("##readonly", text.data(), text.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4), ImGuiInputTextFlags_ReadOnly);

	BaseCamera::Debug();
}

