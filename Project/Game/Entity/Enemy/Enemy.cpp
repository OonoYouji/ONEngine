#include "Enemy.h"

/// engine
#include "Engine/Component/RendererComponents/Sprite/SpriteRenderer.h"


Enemy::Enemy() {}
Enemy::~Enemy() {}


void Enemy::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexturePath("Assets/Textures/enemy.png");

	transform_->scale = Vector3(100.0f, 100.0f, 1.0f); ///< texture size に合わせる

}

void Enemy::Update() {

}
