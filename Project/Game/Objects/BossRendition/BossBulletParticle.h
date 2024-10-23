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

class BossBulletParticle : public BaseGameObject {
public:

	BossBulletParticle() { CreateTag(this); }
	~BossBulletParticle() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	/// <summary>
	///  パーティクル処理
	/// </summary>
	void ParticleInit();
	void ParticleUpdate();

	void SetPivot(Transform pivot);

private:
	// パーティクル
	struct ParticleData {
		float derection_;
		float rotateSpeed;
		int reflectionCount;
		float alphaEaseTime_;
		Transform transform;
		Transform pivot;
		Vec3 rotate{};
		Vec3 velocity{};
	};


	/// 反発係数
	const float reboundFactor_ = -0.5f;
	///　重力
	const float kGravity_ = 20.8f;
	/// パーティクルデータ
	std::vector<ParticleData> particleDataArray_;
	//反射カウント最大
	const int reflectionCountMax_ = 2;
	//透明度イージングタイム

	ParticleSystem* particleSystem_;

	bool isParticle_;

	const float positionOfset_ = 4;

	Transform pivot_;
};