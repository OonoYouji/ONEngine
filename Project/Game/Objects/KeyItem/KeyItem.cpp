#include "KeyItem.h"

/// engine
#include "Engine/ECS/Component/Component.h"

/// game
#include "Game/Effects/KeyItemEffects/KeyItemRipplesEffect.h"
#include "Game/Effects/KeyItemEffects/KeyItemRiseParticle.h"

KeyItem::KeyItem() {}
KeyItem::~KeyItem() {}

void KeyItem::Initialize() {
	//MeshRenderer* renderer = AddComponent<MeshRenderer>();
	//renderer->SetMeshPath("Assets/Models/objects/statue/statue.obj");
	//renderer->SetTexturePath("Packages/Textures/white.png");
	//renderer->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));

	//KeyItemRipplesEffect* ripplesEffect = pEntityComponentSystem_->GenerateEntity<KeyItemRipplesEffect>();
	//ripplesEffect->SetParent(this);

	//KeyItemRiseParticle* riseParticle = pEntityComponentSystem_->GenerateEntity<KeyItemRiseParticle>();
	//riseParticle->SetParent(this);
	//

	transform_->position.y = 1.0f;

}

void KeyItem::Update() {
	// Update the key item state
}