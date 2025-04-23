#include "KeyItem.h"

/// engine
#include "Engine/ECS/Component/Component.h"

/// game
#include "Game/Effects/KeyItemEffects/KeyItemRipplesEffect.h"

KeyItem::KeyItem() {}
KeyItem::~KeyItem() {}

void KeyItem::Initialize() {
	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/primitive/tube.obj");
	renderer->SetTexturePath("Assets/Textures/gradation.png");
	//renderer->SetTexturePath("Packages/Textures/uvChecker.png");

	KeyItemRipplesEffect* ripplesEffect = pEntityComponentSystem_->GenerateEntity<KeyItemRipplesEffect>();
	ripplesEffect->SetParent(this);
}

void KeyItem::Update() {
	// Update the key item state
}