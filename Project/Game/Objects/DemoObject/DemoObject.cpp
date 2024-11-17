#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/Collider/CapsuleCollider.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"


void DemoObject::Initialize() {
	animationRenderer_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	capsuleCollider_ = AddComponent<CapsuleCollider>();
	capsuleCollider_->SetPositionArray({ &positionArray_[0], &positionArray_[1] });
}

void DemoObject::Update() {
	
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("change Kari_Boss_Wait")) {
			animationRenderer_->ChangeAnimation("Kari_Boss_Wait");
		}

		if(ImGui::Button("change Kari_Boss_WeakAttack")) {
			animationRenderer_->ChangeAnimation("Kari_Boss_WeakAttack");
		}

		if(ImGui::Button("change Kari_Boss_StrongAttack")) {
			animationRenderer_->ChangeAnimation("Kari_Boss_StrongAttack");
		}

		if(ImGui::Button("change Kari_Boss_LongWait")) {
			animationRenderer_->ChangeAnimation("Kari_Boss_LongWait");
		}


		ImGui::SeparatorText("capsule collider position array");

		ImGui::DragFloat3("left position", &positionArray_[0].x, 0.1f);
		ImGui::DragFloat3("right position", &positionArray_[1].x, 0.1f);


		ImGui::TreePop();
	}
}
