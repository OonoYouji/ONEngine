#include "TrackingCamera.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// game
#include "Objects/Camera/GameCamera.h"


TrackingCamera::TrackingCamera(GameCamera* _gameCamera, BaseGameObject* _trackingObject)
	: pGameCamera_(_gameCamera),
	trackingObject_(_trackingObject) {

	CreateTag(this);
}

TrackingCamera::~TrackingCamera() {}

void TrackingCamera::Initialize() {

	/// ===================================================
	/// setting
	/// ===================================================

	/// trasform setting
	pGameCamera_->GetTransform()->rotateOrder = QUATERNION;


	/// ===================================================
	/// json variable io
	/// ===================================================

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();

}

void TrackingCamera::Update() {
	ApplyVariables();

	Input();

	/// 座標決定
	Vec3 offset = Mat4::Transform(offsetPosition_, matCameraRotate_);
	pGameCamera_->SetPosition(trackingObject_->GetPosition() + offset);

	LockToTarget();
}


/// ===================================================
/// variable io
/// ===================================================

void TrackingCamera::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offsetPosition", offsetPosition_);
	vm->AddValue(groupName, "offsetDirection", offsetDirection_);
	vm->AddValue(groupName, "rotateSpeed", rotateSpeed_);

}

void TrackingCamera::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	offsetPosition_  = vm->GetValue<Vec3>(groupName, "offsetPosition");
	offsetDirection_ = vm->GetValue<Vec3>(groupName, "offsetDirection");
	rotateSpeed_     = vm->GetValue<Vec2>(groupName, "rotateSpeed");

}



/// ===================================================
/// member methods
/// ===================================================

void TrackingCamera::LockToTarget() {


	Vec3 direction = Mat4::TransformNormal(offsetDirection_, matCameraRotate_);

	/// 回転角決定
	pGameCamera_->SetQuaternion(Quaternion::LockAt({}, direction));

}

void TrackingCamera::Input() {

	inputRightStick_ = Vec2(0, 0);
	inputRightStick_ += Input::MouseVelocity();
	inputRightStick_ += Input::GetRightStick();
	inputRightStick_ = inputRightStick_.Normalize();
	inputRightStick_.y *= -1.0f;

	if(inputRightStick_ != Vec2(0, 0)) {
		inputRightStick_ = inputRightStick_.Normalize() * rotateSpeed_ * Time::DeltaTime();
	}



	cameraRotate_ += {
		inputRightStick_.y, inputRightStick_.x, 0.0f
	};

	matCameraRotate_ = Mat4::MakeRotate(cameraRotate_);

}

