#include "DebugCamera.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

DebugCamera::DebugCamera(DxDevice* _dxDevice) : Camera(_dxDevice) {}
DebugCamera::~DebugCamera() {
	//variables_ = GetComponent<Variables>();
	//variables_->Get<Vector3>("startPos") = transform_->position;
	//variables_->Get<Vector3>("startRot") = transform_->rotate;
	//variables_->SaveJson("./Assets/Jsons/" + GetName() + ".json");
}

void DebugCamera::Initialize() {
	DebugCamera* debugCamera = pEntityComponentSystem_->FindEntity<DebugCamera>();
	if (debugCamera && debugCamera != this) {
		Destroy();
		return;
	}

	AddComponent<CameraComponent>();
	pEntityComponentSystem_->AddDoNotDestroyEntity(this);

	transform_->position = { 0.0f, 0.0f, -10.0f };
	transform_->scale = Vector3::kOne;
	eulerAngles_ = Vector3::kZero;

	fovY_ = 0.7f;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;


	isActive_ = true;
	isMoving_ = false;

	//transform_->position = variables_->Get<Vector3>("startPos");
	//transform_->rotate = variables_->Get<Vector3>("startRot");
	moveSpeed_ = 0.05f; /// 初期値

	UpdateTransform();
	matView_ = transform_->GetMatWorld().Inverse();
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, EngineConfig::kWindowSize.x / EngineConfig::kWindowSize.y,
		nearClip_, farClip_
	);
	viewProjection_->SetMappedData(ViewProjection(matView_ * matProjection_));

}

void DebugCamera::Update() {
#ifdef DEBUG_MODE

	if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_C)) {
		isActive_ = !isActive_;
	}


	if (!isActive_) {
		UpdateTransform();
		return;
	}


	/// カメラが移動していないときだけ判定を取る
	if (!isMoving_) {
		bool isMouseOnScene = true;
		bool isMouseOnPrefab = true;

		/// マウスがSceneウィンドウ内にあるか
		Vector2&& mousePosition = Input::GetImGuiImageMousePosition("Scene");
		if (mousePosition.x < 0.0f || mousePosition.x > 1280.0f ||
			mousePosition.y < 0.0f || mousePosition.y > 720.0f) {
			isMouseOnScene = false;
		}

		mousePosition = Input::GetImGuiImageMousePosition("Prefab");
		if (mousePosition.x < 0.0f || mousePosition.x > 1280.0f ||
			mousePosition.y < 0.0f || mousePosition.y > 720.0f) {
			isMouseOnPrefab = false;
		}

		if (!isMouseOnScene && !isMouseOnPrefab) {
			return;
		}

	}


	/// 移動速度の切り替え
	if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_MINUS)) {
		/// 移動速度を下げる
		moveSpeed_ *= 0.5f;
		if (moveSpeed_ < 0.001f) {
			moveSpeed_ = 0.001f; // 最小値を設定
		}
		Console::Log("[debug] debug camera move speed : " + std::to_string(moveSpeed_));
	}

	if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_EQUALS)) {
		/// 移動速度を上げる
		moveSpeed_ *= 2.0f;
		if (moveSpeed_ > 1.0f) {
			moveSpeed_ = 1.0f; // 最大値を設定
		}
		Console::Log("[debug] debug camera move speed : " + std::to_string(moveSpeed_));
	}



	isMoving_ = false;
	if (Input::PressMouse(Mouse::Right)) {
		isMoving_ = true;

		float speed = moveSpeed_;
		if (Input::PressKey(DIK_LSHIFT)) {
			speed *= 2.0f;
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
		eulerAngles_.y += move.x * 0.01f;
		eulerAngles_.x += move.y * 0.01f;

		transform_->rotate = Quaternion::FromEuler(eulerAngles_);
		transform_->Update();

		matView_ = transform_->GetMatWorld().Inverse();
		matProjection_ = MakePerspectiveFovMatrix(
			fovY_, 1280.0f / 720.0f,
			nearClip_, farClip_
		);
		viewProjection_->SetMappedData(ViewProjection(matView_ * matProjection_));
	}
#endif // DEBUG_MODE

}
