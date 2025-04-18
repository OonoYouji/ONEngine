#include "Player.h"

#define NOMINMAX

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Input/Input.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/entity/player.obj");
	meshRenderer->SetTexturePath("Packages/Textures/uvChecker.png");

	Effect* effect = AddComponent<Effect>();
	effect->SetEmitTypeDistance(10.0f, 4);
	effect->SetMeshPath("Assets/Models/primitive/plane.obj");
	effect->SetTexturePath("Packages/Textures/uvChecker.png");

	effect->SetStartSpeed(0.1f);
	effect->SetLifeLeftTime(0.2f);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			_element->color.w = std::max(0.0f, _element->lifeTime / 3.0f);

			_element->transform.scale.z = 3.0f * (1.0f + (1.0f - _element->lifeTime / 0.2f));

			if (_element->transform.rotate.y == 0) {
				_element->transform.rotate.y = static_cast<float>(rand() % 360) / 180.0f;
			}

			_element->transform.Update();
		}
	);
}

void Player::Update() {

	Vec3& velo = variables_->Get<Vec3>("velocity");
	float& jumpPower = variables_->Get<float>("jumpPower");
	float& speed = variables_->Get<float>("speed");

	velo = Vec3::kZero;

	/// 横移動
	if (Input::PressKey(DIK_W)) { velo.z += 1.0f; }
	if (Input::PressKey(DIK_S)) { velo.z -= 1.0f; }
	if (Input::PressKey(DIK_A)) { velo.x -= 1.0f; }
	if (Input::PressKey(DIK_D)) { velo.x += 1.0f; }

	/// 上下移動
	if (Input::PressKey(DIK_SPACE)) {
		if (jumpPower <= 0.0f) {
			jumpPower = variables_->Get<float>("startJumpPower");
		}
	}

	velo = velo.Normalize() * speed;
	velo.y = jumpPower;
	velo = Matrix4x4::Transform(velo, Matrix4x4::MakeRotateY(transform_->rotate.y));

	/// ジャンプ力の減衰
	jumpPower -= 0.1f;

	/// 移動
	if (velo != Vector3::kZero) {
		/// 移動
		transform_->position += velo;
	}


	/// 地面に着地したら
	if (transform_->position.y < 0.0f) {
		transform_->position.y = 0.0f;
		jumpPower = 0.0f;
	}


}

