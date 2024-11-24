#pragma once

#include "GameObjectManager/BaseGameObject.h"

class DamageNumRender : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DamageNumRender(
		uint32_t _score,
		const Vec3& _startScale,
		const Vec3& _endScale,
		float _lerpTime,
		float _lifeTime,
		class GameCamera* _gameCamera
	);

	~DamageNumRender();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class NumberRenderer* numberRenderer_ = nullptr;

	float lifeTime_;
	uint32_t score_;

	Vec3 startScale_, endScale_;
	float lerpTime_, lerpMaxTime_;

	Quaternion defaultQuaternionY_;
	class GameCamera* pMainCamera_ = nullptr;

};
