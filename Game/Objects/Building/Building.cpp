#include "Building.h"

#include <ImGuiManager.h>
#include <ModelManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/Collider/BoxCollider.h>


void Building::Initialize() {

	/// component
	MeshRenderer* mr = AddComponent<MeshRenderer>();
	mr->SetModel("TestObject");

	BoxCollider*  bc = AddComponent<BoxCollider>(ModelManager::Load("TestObject"));
	
	

	/// parent
	pivot_.Initialize();
	pivot_.rotateOrder       = QUATERNION;

	/// transform
	pTransform_->position.z  = -10.0f;
	pTransform_->rotateOrder = QUATERNION;
	SetParent(&pivot_);


}

void Building::Update() {

	quaternionX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateRateX_);
	quaternionY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateRateY_);

	pivot_.quaternion = quaternionX_ * quaternionY_;
	pivot_.UpdateMatrix();

}

void Building::Debug() {
	if(!ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	if(ImGui::TreeNodeEx("pivot", ImGuiTreeNodeFlags_DefaultOpen)) {

		/// quaernion theta
		ImGui::DragFloat("rotateRateX", &rotateRateX_, 0.01f);
		ImGui::DragFloat("rotateRateY", &rotateRateY_, 0.01f);

		/// quaternion debug draw
		ImGui::DragFloat4("quaternionX", &quaternionX_.x, 0.0f);
		ImGui::DragFloat4("quaternionY", &quaternionY_.x, 0.0f);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 wPos = GetPosition();
		ImGui::DragFloat3("world position", &wPos.x, 0.0f);

		ImGui::TreePop();
	}


	ImGui::TreePop();
}
