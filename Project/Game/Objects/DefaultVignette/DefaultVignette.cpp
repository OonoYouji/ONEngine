#include "DefaultVignette.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Scenes/Scene_Game.h"


DefaultVignette::DefaultVignette() {
	CreateTag(this);
}

DefaultVignette::~DefaultVignette() {}

void DefaultVignette::Initialize() {
	drawLayerId = GAME_SCENE_LAYER_UI;

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("playerDamage.png");

	pTransform_->position.z = 4.0f;
	pTransform_->scale    = { 
		8.2f, 4.6f, 0.0f
	};

}

void DefaultVignette::Update() {

}

