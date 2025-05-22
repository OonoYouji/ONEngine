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
	laser_ = pEntityComponentSystem_->GenerateEntity<LaserEffect>();
	laser_->SetParent(this);
	laser_->SetActive(false);

	LaserHitEffect* laserHit = pEntityComponentSystem_->GenerateEntity<LaserHitEffect>();



	/// 子オブジェクトを生成する
	PuzzleClearEffectCannonStand* cannonStand = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannonStand>();
	cannonStand->SetParent(this);

	cannon_ = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannon>();
	cannon_->SetParent(this);
	cannon_->SetActive(false);


	variables_->Add<bool>("isStart", false);
}

void PuzzleClearEffect::Update() {

	bool& isStart = variables_->Get<bool>("isStart");

	if (!isStart) {
		if (Input::TriggerKey(DIK_SPACE)) {
			isStart = true;
			cannon_->SetActive(true);
		}
		cannon_->UpdateTransform(); /// 座標が更新されないと困るので
		laser_->UpdateTransform();
		return;
	}



	bool& endCannonAnime = variables_->Get<bool>("endCannonAnime");

	Variables* cannonVars = cannon_->GetComponent<Variables>();
	float& cannonAnimeTime = cannonVars->Get<float>("animeTime");

	if (cannonAnimeTime > 2.0f) {
		endCannonAnime = true;
		laser_->SetActive(true);
	}

}
