#include "Enemy.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Enemy");
	meshRenderer_->SetMaterial("white2x2");
	meshRenderer_->SetColor(Vec4::kRed);


}

void Enemy::Update() {

}
