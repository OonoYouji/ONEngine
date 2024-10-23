#include "PlayerBullet.h"

/// engine
#include <imgui.h>
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"

PlayerBullet::PlayerBullet() {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {

	Model* model = ModelManager::Load("Sphere");

	/// renderer
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);


	/// collider
	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);

}

void PlayerBullet::Update() {
	pTransform_->position += velocity_;

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
		collision->Destory();
		Destory();
	}
}

void PlayerBullet::SetVelocity(const Vec3& _velocity) {
	velocity_ = _velocity;
}
