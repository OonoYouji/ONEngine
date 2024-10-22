#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/ParticleSystem/ParticleSystem.h>
//std
#include<memory>
//behavior

#include"Objects/Boss/BossVacuum.h"

class BossAttackParticle : public BaseGameObject {
public:

	BossAttackParticle() { CreateTag(this); }
	~BossAttackParticle() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	struct ParticleData {
		float time = 0.0f;
		float rotateSpeed = 1.0f;
		float radius = 1.0f;
		float maxPosY = 0.0f;
		float reflectionCount;
		Vec3 rotate{};
		Vec3 velocity{};
		/*Transform pivot;*/
	};


	/// 反発係数
	const float reboundFactor_ = -0.6f;
	
	const float kGravity_=4.8f;
	std::vector<ParticleData> particleDataArray_;
	//Transform pivot_;
};