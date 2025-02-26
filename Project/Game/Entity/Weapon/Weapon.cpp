#include "Weapon.h"

/// engine
#include "Engine/Component/RendererComponents/Sprite/SpriteRenderer.h"


Weapon::Weapon() {}
Weapon::~Weapon() {}


void Weapon::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexturePath("Assets/Textures/sword.png");

	transform_->scale = Vector3(100.0f, 100.0f, 1.0f); ///< texture size に合わせる

}

void Weapon::Update() {

}
