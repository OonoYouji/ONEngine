#include "PuzzleClearEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"

#include "Cannon/PuzzleClearEffectCannon.h"
#include "Cannon/PuzzleClearEffectCannonStand.h"


PuzzleClearEffect::PuzzleClearEffect() {}
PuzzleClearEffect::~PuzzleClearEffect() {}

void PuzzleClearEffect::Initialize() {

	Effect* effect = AddComponent<Effect>();
	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetMaxEffectCount(1024);
	effect->SetMeshPath("Assets/Models/objects/PuzzleClearEffect/PuzzleClearEffectLaser.obj");
	effect->SetTexturePath("Assets/Textures/circle.png");
	effect->SetLifeLeftTime(2.0f);

	effect->SetStartSpeed(100.0f);
	effect->SetStartSize(Vector3::kOne);
	effect->SetStartColor(Color(0.75f, 0.75f, 0.75f, 0.2f));

	effect->SetEmitTypeTime({ .emitTime = 0.01f, .emitInterval = 0.0f }, 7);
	effect->SetEmitShape({ 0, 0, 0 }, Vec3::kZero);

	effect->SetElementUpdateFunc(
		[this](Effect::Element* _element) {
			Vector3& direction = variables_->Get<Vector3>("effectDirection");
			_element->transform.position += Matrix4x4::Transform(direction, transform_->matWorld) * Time::DeltaTime();
		}
	);



	/// 子オブジェクトを生成する
	PuzzleClearEffectCannonStand* cannonStand = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannonStand>();
	cannonStand->SetParent(this);

	PuzzleClearEffectCannon* cannon = pEntityComponentSystem_->GenerateEntity<PuzzleClearEffectCannon>();
	cannon->SetParent(cannonStand);

}

void PuzzleClearEffect::Update() {

}
