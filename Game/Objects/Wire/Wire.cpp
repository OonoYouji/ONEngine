#include "Wire.h"

#include <ModelManager.h>
#include <ImGuiManager.h>



void Wire::Initialize() {
	model_ = ModelManager::Load("Wire");
}

void Wire::Update() {

}

void Wire::Draw() {
	model_->Draw(&transform_);
}

void Wire::Debug() {
	if(ImGui::TreeNodeEx("top bottom", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("top", &top_.x);
		ImGui::DragFloat3("bottom", &bottom_.x);

		ImGui::TreePop();
	}
}
