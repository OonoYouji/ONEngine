#include "Enemy.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"



Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	Model* model = ModelManager::Load("Enemy");

	/// renderer
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("white2x2");
	meshRenderer_->SetColor(Vec4::kRed);


	/// collider
	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);

}

void Enemy::Update() {

}
