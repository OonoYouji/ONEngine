#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TrackingCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TrackingCamera(class GameCamera* _gameCamera, BaseGameObject* _trackingObject);
	~TrackingCamera();

	void Initialize() override;
	void Update()     override;

	/// <summary>
	/// json io
	/// </summary>
	void AddVariables();
	void ApplyVariables();


	void LockToTarget();

	void Input();

	Vec3 LockAt(const Vec3& _direction);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GameCamera*     pGameCamera_    = nullptr;
	class BaseGameObject* trackingObject_ = nullptr;

	Vec3 offsetPosition_;  /// 追従対象からの距離
	Vec3 offsetDirection_; /// 見る方向

	Vec3  cameraRotate_; /// カメラの回転角
	Mat4  matCameraRotate_;
	Vec2  rotateSpeed_;  /// カメラの回転速度


	Vec2 inputRightStick_;

};
