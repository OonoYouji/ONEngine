#include <DebugCamera.h>

#include <CameraManager.h>
#include <Input.h>
#include <ImGuiManager.h>


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

	//transform_.position = { 0.0f, 0.0f, -6.49f * 2 };
	/*transform_.position = { 1.8f,0.87f, -12.7f };
	transform_.rotate = { 0.066f, -0.258f, 0.0f };*/

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
	velocity_.x *= 1.0f + transform_.position.Len() / 32.0f;
	velocity_.y *= 1.0f + transform_.position.Len() / 32.0f;

	if(Input::PressMouse(MouseCode::Right)) {
		transform_.rotate.x += Input::MouseVelocity().y / 256.0f;
		transform_.rotate.y += Input::MouseVelocity().x / 256.0f;
	}

	velocity_ = Mat4::Transform(velocity_, Mat4::MakeRotate(transform_.rotate));
	transform_.position += velocity_;

	transform_.UpdateMatrix();
	matView_ = transform_.matTransform.Inverse();
	matVp_ = matView_ * matProjection_;

	Transfer();
}

