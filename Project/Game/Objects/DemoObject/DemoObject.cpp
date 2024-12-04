#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {
	auto renderer = AddComponent<MeshRenderer>();
	renderer->SetModel("Field");

	axis_ = Vec3(1.0f, 1.0f, 1.0f).Normalize();
	angle_ = 0.44f;

	matRotata_ = Quaternion::MakeRotateAxisAngle(axis_, angle_);
}

void DemoObject::Update() {
	
	matRotata_ = Quaternion::MakeRotateAxisAngle(axis_, angle_);

	//pTransform_->quaternion = Quaternion::MakeFromAxis(axis_, angle_);
}

void DemoObject::Debug() {

	if(ImGui::TreeNodeEx("MT4_01_01", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::DragFloat3("axis", &axis_.x, 0.025f)) {
			axis_ = axis_.Normalize();
		}

		ImGui::DragFloat("angle", &angle_);
	

		ImGui::Text("rotate matrix");
		for(size_t r = 0; r < 4; ++r) {
			for(size_t c = 0; c < 4; ++c) {
				if(c != 0) {
					ImGui::SameLine();
				}
					
				ImGui::Text(std::format("{:0.3f},", matRotata_.m[r][c]).c_str());

			}

		}

		ImGui::TreePop();
	}
}

