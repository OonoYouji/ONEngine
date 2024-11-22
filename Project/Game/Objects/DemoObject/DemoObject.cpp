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
	/*animationRenderer_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	capsuleCollider_ = AddComponent<CapsuleCollider>();
	capsuleCollider_->SetPositionArray({ &positionArray_[0], &positionArray_[1] });*/

	VariableManager* vm = VariableManager::GetInstance();
	vm->AddValue(GetTag(), "name", name_);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
}

void DemoObject::Update() {

	VariableManager* vm = VariableManager::GetInstance();
	name_ = vm->GetValue<std::string>(GetTag(), "name");




	isCollisionEnter_ = false;
	isCollisionStay_ = false;
	isCollisionExit_ = false;


}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		static char buff[256];
		ImGui::InputText("test text", buff, sizeof(buff));

		ImGuiIO& io = ImGui::GetIO();
		ImGui::DragFloat("repeat delay", &io.KeyRepeatDelay, 0.01f);
		ImGui::DragFloat("repeat rate", &io.KeyRepeatRate, 0.01f);
		

		ImGui::Checkbox("isCollisionEnter", &isCollisionEnter_);
		ImGui::Checkbox("isCollisionStay", &isCollisionStay_);
		ImGui::Checkbox("isCollisionExit", &isCollisionExit_);

		ImGui::TreePop();
	}
}

void DemoObject::OnCollisionEnter(BaseGameObject* const _collision) {
	isCollisionEnter_ = true;
}

void DemoObject::OnCollisionStay(BaseGameObject* const _collision) {
	isCollisionStay_ = true;
}

void DemoObject::OnCollisionExit(BaseGameObject* const _collision) {
	isCollisionExit_ = true;
}
