#pragma once

#include "GameObjectManager/BaseGameObject.h"

class HitEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	HitEffect(class GameCamera* _camera);
	~HitEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GameCamera* pCamera_ = nullptr;

	class MeshRenderer* meshRenderer_ = nullptr;

	float time_;
	float frameDuration_;

	int frameIndex_;

};
