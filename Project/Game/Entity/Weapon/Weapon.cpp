#include "Weapon.h"

/// engine
#include "Engine/Component/RendererComponents/Sprite/SpriteRenderer.h"


Weapon::Weapon() {}
Weapon::~Weapon() {}


void Weapon::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexturePath("Assets/Textures/sword.png");

}

void Weapon::Update() {

}
