#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBullet : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBullet(class TopDownCamera* _topDownCamera);
	~PlayerBullet();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;

	/// <summary>
	/// 弾を発射したあと初期化用の設定
	/// </summary>
	/// <param name="_velocity">    : 移動先の速度 </param>
	/// <param name="_maxLifeTime"> : 寿命        </param>
	void StartupSetting(const Vec3& _velocity, float _maxLifeTime);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class PlayerBulletHitEffect* effect_         = nullptr;
	class TopDownCamera*         pTopDownCamera_ = nullptr;

	Vec3  velocity_;
	float lifeTime_;
	bool  isAlive_;

	bool isHit_ = false;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	bool GetIsAlive() const { return isAlive_; }

	bool GetIsHit() const { return isHit_; }

};
