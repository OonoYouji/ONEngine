#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Boss;
/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class BossBulletLump final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	BossBulletLump() { CreateTag(this); }
	~BossBulletLump() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;
	//getter
	bool GetIsDeath()const { return isDeath_; }

	//setter^d
	void SetDirection(Quaternion direction);
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetQuaternionSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }
	void SetBoss(Boss* boss);

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;
private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	Transform pivot_;
	bool isDeath_;
	//弾を発射した瞬間に当たらないように一応無敵時間
	float invincibleTime_;
	const float kInvincibleTime_=1.0f;
	/// audio source
	class AudioSource* audioSource_ = nullptr;
};
