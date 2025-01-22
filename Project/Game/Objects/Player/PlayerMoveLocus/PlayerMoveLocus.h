#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerMoveLocus : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMoveLocus(class Player* _player);
	~PlayerMoveLocus();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshInstancingRenderer* renderer_ = nullptr;
	class Player*                 pPlayer_  = nullptr;

	using TransformWithLifeTime = std::pair<float, Transform>;
	std::vector<TransformWithLifeTime> transformWithLifeTimes_;

	float durationTime_ = 0.1f;
	float currentTime_  = 0.0f;

	float timeSubScalerValue_ = 0.0f;

};
