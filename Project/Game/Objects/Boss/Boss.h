#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
//std
#include<memory>
//behavior
#include"Objects/BossBehavior/BaseBossBehavior.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossChasePlayer.h"
#include"Objects/BossBehavior/BossBulletShot.h"
#include <ComponentManager/ParticleSystem/ParticleSystem.h>
#include"Objects/BossBehavior/BossAttack.h"

#include"Objects/Boss/BossVacuum.h"

class Player;
class BuildingManager;
class BaseBuilding;
class BossTubu;
class BossHead;
//class BossHead;
class Boss : public BaseGameObject {
public:

	Boss() { CreateTag(this); }
	~Boss() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//通常更新
	void RootInit();
	void RootUpdate();
	//ストーカー
	void ChaseInit();
	void ChaseUpdate();
	//建物吸引
	void SlurpInit();
	void SlurpUpdate();
	//弾発射
	void BulletShotInit();
	void BulletShotUpdate();
	//攻撃
	void AttackInit();
	void AttackUpdate();
	void AttackFixationUpdate();

	void DamageForPar(const float& par);

	//プレイヤーセット
	void SetPlayer(Player* player);
	void SetHead(BossHead* bossHead);
	void SetTubu(BossTubu* bosstube);
	void SetBuildingaManager(BuildingManager* player);
	BaseBuilding* FindClosestBuilding();

	//状態変更
	void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	//getter
	bool GetIsSlurping()const { return isSlurping_; }
	bool GetIsHitBack()const { return isHitBack_; }
	float GetChaseSpeedParamater()const { return SpeedParamater_; }
	Quaternion GetPivotQuaternion()const { return pivot_.quaternion; }
	bool GetIsBuildingKill()const { return isBuildingKill_; }
	Player* GetPlayer() { return pPlayer_; }
	BuildingManager* GetBuildingManager()const { return pBuildingManager_; }

	void OnCollisionStay([[maybe_unused]] BaseGameObject* const collision)override;
	bool GetIsAttack()const { return isAttack_; }

	//setter
	void SetIsSlurping(bool is) { isSlurping_ = is; }
	void SetSlurpingCoolTimer() { slurpCooldownTimer_ = kSlurpCollTime_; }
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetPivotSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }
	void SetIsBuildingKill(bool is) { isBuildingKill_ = is; }

	void SetIsAttack(bool is) { isAttack_ = is; }

/// <summary>
/// Particle
/// </summary>
	void ParticleInit();
	void ParticleUpdate();

private:
	
private:
	
	MeshRenderer* meshRenderer_ = nullptr;
	//プレイヤーポインタ
	Player* pPlayer_ = nullptr;
	BuildingManager* pBuildingManager_ = nullptr;
	BossTubu* pBossTubu_;
	BossHead* pBossHead_;
	//ライト
	class EarthRenderer* er_ = nullptr;
	float radius_ = 1.0f;
	Vec4  paintOutColor_ = { 1,1,1,1 };
	////状態
	std::unique_ptr<BaseBossBehavior>behavior_ = nullptr;
	//ピボット
	Transform pivot_;
	//

	//吸ってる弾を殺すフラグ
	bool isBuildingKill_;
	//攻撃フラグ
	bool isAttack_;
	////////////////////////////////////////////////////////////////////////////////////////////
	//  パラメータ
	////////////////////////////////////////////////////////////////////////////////////////////
	//パラメータ調節用追従すピーⅮ
	float SpeedParamater_;
	//吸い込み関連
	bool isSlurping_ = false;  // 吸い込み中かどうか
	float slurpCooldownTimer_ = 0.0f;  // クールダウン用のタイマー
	const float kSlurpCollTime_ = 1.0f;  // 吸い込み完了後のクールダウン時間（秒）

	//ボス弾発射関連
	const uint32_t kBuildingNum_ = 10;

	//ひどい変数群(攻撃イージング)
	bool isAttackBack_;
	float attackEaseT_;
	float attackCoolTime_;
	const float kAttackCoolTime_ = 0.5f;
	const float kAttackEaseT_ = 0.5f;

	//HP
	float HP_;
	float HPMax_;

	//ダメージ受けたー
	bool isHitBack_;
	float damageCoolTime_;
	const	float kDamageCoolTime_ = 0.1f;
	float nextDamageCollTime_;//次にダメージ受けるまでのクールタイム
	float nextDamageTime_;//次にダメージ受けるまでのクールタイム

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
	const int reflectionCountMax_ = 4;
	//透明度イージングタイム

	ParticleSystem* particleSystem_;

	bool isParticle_;

	const float positionOfset_=2;

	class AudioSource* audioSource_ = nullptr;

	
};