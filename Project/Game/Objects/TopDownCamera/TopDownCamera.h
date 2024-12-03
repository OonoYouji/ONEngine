#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TopDownCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TopDownCamera(class GameCamera* _gameCamera, BaseGameObject* _targetObject);
	~TopDownCamera();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void ApplyVariables();

	void StartShake(float _minValue, float _maxValue, float _time);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GameCamera* pGameCamera_  = nullptr;
	BaseGameObject*   targetObject_ = nullptr;

	Vec3 offsetPosition_;
	Vec3 offsetRotate_;


	Vec3  shakeValue_;
	float shakeTime_, shakeMaxTime_;
	float shakeMinValue_, shakeMaxValue_;

};
