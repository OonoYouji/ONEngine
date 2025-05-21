#include "PuzzleClearEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"

#include "Cannon/PuzzleClearEffectCannon.h"
#include "Cannon/PuzzleClearEffectCannonStand.h"
#include "Effect/LaserEffect.h"

PuzzleClearEffect::PuzzleClearEffect() {}
PuzzleClearEffect::~PuzzleClearEffect() {}

void PuzzleClearEffect::Initialize() {

	/// エフェクトの生成
	LaserEffect* laserEffect = pEntityComponentSystem_->GenerateEntity<LaserEffect>();
	laserEffect->SetParent(this);



	/// 子オブジェクトを生成する
	PuzzleClearEffectCannonStand* cannonStand = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannonStand>();
	cannonStand->SetParent(this);

	PuzzleClearEffectCannon* cannon = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannon>();
	cannon->SetParent(this);

}

void PuzzleClearEffect::Update() {

}
