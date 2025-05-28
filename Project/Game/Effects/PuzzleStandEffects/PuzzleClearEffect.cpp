#include "PuzzleClearEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"

#include "Cannon/PuzzleClearEffectCannon.h"
#include "Cannon/PuzzleClearEffectCannonStand.h"
#include "Effect/LaserEffect.h"
#include "Effect/LaserHitEffect.h"

PuzzleClearEffect::PuzzleClearEffect() {}
PuzzleClearEffect::~PuzzleClearEffect() {}

void PuzzleClearEffect::Initialize() {

	///// エフェクトの生成
	laserEffects_[0] = pEntityComponentSystem_->GenerateEntity<LaserEffect>();
	laserEffects_[1] = pEntityComponentSystem_->GenerateEntity<LaserShootEffect>();

	for (auto& laserEffect : laserEffects_) {
		laserEffect->SetParent(this);
		laserEffect->SetActive(false);
	}

	hitEffects_[0] = pEntityComponentSystem_->GenerateEntity<LaserHitEffect>();
	hitEffects_[1] = pEntityComponentSystem_->GenerateEntity<LaserExplosionEffect>();

	for (auto& hitEffect : hitEffects_) {
		hitEffect->SetParent(this);
		hitEffect->SetActive(false);
	}


	/// 子オブジェクトを生成する
	PuzzleClearEffectCannonStand* cannonStand = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannonStand>();
	cannonStand->SetParent(this);

	cannon_ = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannon>();
	cannon_->SetParent(this);
	cannon_->SetActive(false);


	AddComponent<ToTerrainCollider>();


	//variables_->Add<bool>("isStart", false);
	//variables_->Add<float>("timeToImpact", 0.0f);
}

void PuzzleClearEffect::Update() {

	if (Input::TriggerKey(DIK_R)) {
		Reset();
	}


	bool& isStart = variables_->Get<bool>("isStart");

	if (!isStart) {
		if (Input::TriggerKey(DIK_SPACE)) {
			isStart = true;
			cannon_->SetActive(true);
		}
		cannon_->UpdateTransform(); /// 座標が更新されないと困るので

		for (auto& laserEffect : laserEffects_) {
			laserEffect->UpdateTransform();
		}
		return;
	}



	bool& endCannonAnime = variables_->Get<bool>("endCannonAnime");

	Variables* cannonVars = cannon_->GetComponent<Variables>();
	float& cannonAnimeTime = cannonVars->Get<float>("animeTime");

	if (cannonAnimeTime > 2.0f) {
		endCannonAnime = true;
		
		laserEffects_[0]->SetActive(true);
		IEntity* shootEffect = laserEffects_[1];
		Variables* vars = shootEffect->GetComponent<Variables>();
		if (vars->Get<float>("animeTime") < 0.5f) {
			shootEffect->SetActive(true);
		}

	}

	if (endCannonAnime) {
		float& timeToImpact = variables_->Get<float>("timeToImpact");
		timeToImpact -= Time::DeltaTime();
		if (timeToImpact <= 0.0f) {
			Variables* laserVars = laserEffects_[0]->GetComponent<Variables>();
			const Vector3& cannonDirection = laserVars->Get<Vector3>("effectDirection");
			for (auto& hitEffect : hitEffects_) {
				hitEffect->SetActive(true);
				hitEffect->SetPosition(cannonDirection.Normalize() * (100.0f * 0.5f + 10.0f) + Vector3(0, 1.2f, 0));
				hitEffect->UpdateTransform();
			}
		}
	}

}

void PuzzleClearEffect::Reset() {
	Variables* shootVars = laserEffects_[1]->GetComponent<Variables>();
	shootVars->Get<float>("animeTime") = 0.0f;

	for (auto& laserEffect : laserEffects_) {
		laserEffect->SetActive(false);
	}
	cannon_->SetActive(false);
	for (auto& hitEffect : hitEffects_) {
		hitEffect->SetActive(false);
	}
	Variables* cannonVars = cannon_->GetComponent<Variables>();
	cannonVars->Get<float>("animeTime") = 0.0f;
	variables_->Get<bool>("isStart") = false;
	variables_->Get<bool>("endCannonAnime") = false;
	variables_->Get<float>("timeToImpact") = 0.5f;
}
