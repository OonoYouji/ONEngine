#include "GameTilteRendere.h"

/// engine
#include "imgui.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


void GameTitleRenderer::Initialize() {

	/// draw layer
	drawLayerId = 1; /// ui

	/// add components
	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("title.png");



}


void GameTitleRenderer::Update() {

}


void GameTitleRenderer::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}
