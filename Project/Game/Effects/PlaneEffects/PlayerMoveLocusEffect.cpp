#include "PlayerMoveLocusEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"

void PlayerMoveLocusEffect::Initialize() {
	Effect* effect = AddComponent<Effect>();
	effect->SetMeshPath("Assets/Models/objects/sideToPlane/sideToPlane.obj");
	effect->SetTexturePath("Assets/Textures/ring.png");

	effect->SetEmitShape(Vec3::kZero, Vec3(0.0f, 0.5f, 0.0f));
	effect->SetStartSpeed(0.0f);
	effect->SetLifeLeftTime(1.0f);
	effect->SetStartSize(Vec3::kOne);
	effect->SetStartRotate(Vector3::kZero);
	effect->SetStartColor(Color(1.0f, 1.0f, 1.0f, 0.5f), Color(0.0f, 1.0f, 1.0f, 0.5f));
	effect->SetMaxEffectCount(64);
	effect->SetEmitTypeDistance(0.01f, 1);
	effect->SetBlendMode(Effect::BlendMode::Add);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			_element->color.a = std::lerp(0.0f, _element->color.a, _element->lifeTime / 1.0f);
		}
	);

}

void PlayerMoveLocusEffect::Update() {
	Effect* effect = GetComponent<Effect>();

	// ワールドポジションを取得  
	Vec3& prevPos = variables_->Get<Vec3>("prevPos");
	
	// 移動方向を計算 (例: x方向の変化量を使用)  
	Vec3 toFront = prevPos - GetWorldPosition();
	float x = std::atan2(toFront.z, toFront.x);
	float y = std::atan2(toFront.x, toFront.z);

	// yを移動方向に基づいて設定  
	effect->SetStartRotate(Vector3(x, y, 0.0f));

	prevPos = GetWorldPosition();
}