#include "PlayerBullet.h"

/// engine
#include <imgui.h>
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"

/// objects
#include "Objects/Enemy/Enemy.h"
#include "Objects/Player/Player.h"
#include "Objects/DefeatedEnemy/DefeatedEnemy.h"


PlayerBullet::PlayerBullet(Player* _player, class DefeatedEnemy* _defeatedEnemy) : pPlayer_(_player), pDefeatedEnemy_(_defeatedEnemy) {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {


	/// renderer
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Bullet");


	/// collider
	Model* model = ModelManager::Load("BulletCollider");
	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);

	pTransform_->rotateOrder = QUATERNION;
}

void PlayerBullet::Update() {
	pTransform_->position += velocity_;
	pTransform_->quaternion = Quaternion::LockAt({}, velocity_);

	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}
}

void PlayerBullet::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("velocity", &velocity_.x, 0.05f);

		ImGui::TreePop();
	}
}

void PlayerBullet::OnCollisionEnter(BaseGameObject* const collision) {

	/// 敵の衝突したら敵と自分のインスタンスを消去する
	if(collision->GetTag() == "Enemy") {
		Enemy* enemy = static_cast<Enemy*>(collision);
		enemy->SubHP(1.0f); /// hpを減らす

		/// 死んだらオブジェクトを削除する
		if(enemy->GetHP() <= 0.0f) {
			enemy->Destory();

			pPlayer_->AddScore(50u);
			pDefeatedEnemy_->StartRotate();
		}
		Destory();
	}
}

void PlayerBullet::SetVelocity(const Vec3& _velocity) {
	velocity_ = _velocity;
}
