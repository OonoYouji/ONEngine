#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBullet : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBullet();
	~PlayerBullet();

	void Initialize() override;
	void Update()     override;

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

	Vec3  velocity_;
	float lifeTime_;
	bool  isAlive_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	bool GetIsAlive() const { return isAlive_; }

};
