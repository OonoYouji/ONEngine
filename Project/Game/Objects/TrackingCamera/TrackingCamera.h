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
	void Debug()      override;


	/// <summary>
	/// json io
	/// </summary>
	void AddVariables();
	void ApplyVariables();


	void LockToTarget();

	void Input();

	Vec3 LockAt(const Vec3& _direction);

	void StartShake(float _minValue, float _maxValue, float _time);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GameCamera*     pGameCamera_    = nullptr;
	class BaseGameObject* trackingObject_ = nullptr;
	class Player*         pPlayer_        = nullptr;

	Vec3 offsetPosition_;  /// 追従対象からの距離
	Vec3 offsetDirection_; /// 見る方向

	Vec3  cameraRotate_; /// カメラの回転角
	Mat4  matCameraRotate_;
	Vec2  rotateSpeed_;  /// カメラの回転速度


	Vec2 inputRightStick_;

	/// shake
	float shakeMinValue_;
	float shakeMaxValue_;
	float shakeMaxTime_;
	float shakeTime_;

	Vec3 shakeOffset_;

};
