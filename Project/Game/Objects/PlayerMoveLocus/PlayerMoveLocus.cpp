#include "PlayerMoveLocus.h"

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// user
#include "Objects/Player/Player.h"

PlayerMoveLocus::PlayerMoveLocus(Player* _player)
	: pPlayer_(_player) {
	CreateTag(this);
}

PlayerMoveLocus::~PlayerMoveLocus() {}

void PlayerMoveLocus::Initialize() {

	renderer_ = AddComponent<MeshInstancingRenderer>(16);
	renderer_->SetModel("Cube");


}

void PlayerMoveLocus::Update() {

	if(pPlayer_->GetDirection() == Vector3()) {
		return;
	}

	const Vec3& velocity = pPlayer_->GetVelocity();
	Mat4 matRotate = Mat4::MakeRotateY(pPlayer_->GetMesh()->GetRotate().y);
	for(size_t i = 0; i < 16; ++i) {
		transforms_[i].position = -velocity * 10.0f * Vec3(0,0, static_cast<float>(i) / 15.0f);
		transforms_[i].position = Mat4::Transform(transforms_[i].position, matRotate);
		transforms_[i].position += pPlayer_->GetPosition();
		transforms_[i].position.y += 2.0f;

		transforms_[i].scale = Vec3::kOne * 0.5f * (1.0f - (static_cast<float>(i) / 15.0f));
		transforms_[i].Update();
	}

	renderer_->ResetTransformArray();
	for(auto& transform : transforms_) {
		renderer_->AddTransform(&transform);
	}

}

