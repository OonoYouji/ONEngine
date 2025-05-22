#include "DebugCamera.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

DebugCamera::DebugCamera(DxDevice* _dxDevice) : Camera(_dxDevice) {}
DebugCamera::~DebugCamera() {}

void DebugCamera::Initialize() {

	transform_->position = { 0.0f, 0.0f, -10.0f };
	transform_->scale = Vector3::kOne;
	transform_->rotate = Vector3::kZero;

	fovY_ = 0.7f;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;


	isMoving_ = false;

	UpdateTransform();
	matView_ = transform_->GetMatWorld().Inverse();
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, 1280.0f / 720.0f,
		nearClip_, farClip_
	);
	viewProjection_->SetMappedData(ViewProjection(matView_ * matProjection_));
}

void DebugCamera::Update() {
#ifdef _DEBUG



	/// カメラが移動していないときだけ判定を取る
	if (!isMoving_) {
		/// マウスがSceneウィンドウ内にあるか
		const Vector2& mousePosition = Input::GetImGuiImageMousePosition("Scene");
		if (mousePosition.x < 0.0f || mousePosition.x > 1280.0f ||
			mousePosition.y < 0.0f || mousePosition.y > 720.0f) {
			return;
		}
	}


	isMoving_ = false;
	if (Input::PressMouse(Mouse::Right)) {
		isMoving_ = true;

		float speed = 0.1f;
		if (Input::PressKey(DIK_LSHIFT)) {
			speed *= 5.0f;
		}


		Vector3 velocity = Vector3::kZero;
		if (Input::PressKey(DIK_W)) { velocity.z += speed; }
		if (Input::PressKey(DIK_S)) { velocity.z -= speed; }
		if (Input::PressKey(DIK_A)) { velocity.x -= speed; }
		if (Input::PressKey(DIK_D)) { velocity.x += speed; }
		if (Input::PressKey(DIK_E)) { velocity.y += speed; }
		if (Input::PressKey(DIK_Q)) { velocity.y -= speed; }

		velocity = Matrix4x4::Transform(velocity, Matrix4x4::MakeRotate(transform_->rotate));
		transform_->position += velocity * 10.0f;

		const Vector2& move = Input::GetMouseVelocity();
		transform_->rotate.y += move.x * 0.01f;
		transform_->rotate.x += move.y * 0.01f;

		transform_->Update();

		matView_ = transform_->GetMatWorld().Inverse();
		matProjection_ = MakePerspectiveFovMatrix(
			fovY_, 1280.0f / 720.0f,
			nearClip_, farClip_
		);
		viewProjection_->SetMappedData(ViewProjection(matView_ * matProjection_));
	}
#endif // _DEBUG

}
