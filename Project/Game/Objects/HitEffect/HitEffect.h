#pragma once

#include "GameObjectManager/BaseGameObject.h"

class HitEffect : public BaseGameObject {

	struct EffectElement {
		Transform transform;
		Vec3      velocity;
	};

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
	class MeshInstancingRenderer* meshInstancingRenderer_ = nullptr;

	float time_;
	float frameDuration_;

	int frameIndex_;
	int frameMaxIndex_;

	std::vector<EffectElement> elements_;

};