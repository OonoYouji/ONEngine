#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

#include "Input/Input.h"

/// components
#include "ComponentManager/Collider/CapsuleCollider.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"


void DemoObject::Initialize() {
	animationRenderer_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

}

void DemoObject::Update() {
	
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(name_.c_str());


		static char buff[256];
		ImGui::InputText("test text", buff, sizeof(buff));

		ImGuiIO& io = ImGui::GetIO();
		ImGui::DragFloat("repeat delay", &io.KeyRepeatDelay, 0.01f);
		ImGui::DragFloat("repeat rate", &io.KeyRepeatRate, 0.01f);


		if(Input::GamepadConnected(0)) {
			ImGui::Text("gamepad connected");
		} else {
			ImGui::Text("gamepad found");
		}

		/*if(ImGui::Button("change Kari_Boss_Wait")) {
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


		ImGui::TreePop();
	}
}
