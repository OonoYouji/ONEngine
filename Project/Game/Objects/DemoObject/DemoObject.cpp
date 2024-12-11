#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {

	from0 = Vec3::Normalize({ 1.0f, 0.7f, 0.5f });
	to0 = -from0;

	from1 = Vec3::Normalize({ -0.6f, 0.9f, 0.2f });
	to1 = Vec3::Normalize({ 0.4f, 0.7f, -0.5f });

	matRotate0 = Mat4::DirectionToDirection(Vec3::Normalize({1.0f, 0.0f, 0.0f}), Vec3::Normalize({-1.0f, 0.0f, 0.0f}));
	matRotate1 = Mat4::DirectionToDirection(from0, to0);
	matRotate2 = Mat4::DirectionToDirection(from1, to1);

}

void DemoObject::Update() {


}

void DemoObject::Debug() {




	ImGui::Spacing();
	ImGui::Text("matRotate0");
	TextMatrix(matRotate0);

	ImGui::Spacing();
	ImGui::Text("matRotate1");
	TextMatrix(matRotate1);

	ImGui::Spacing();
	ImGui::Text("matRotate2");
	TextMatrix(matRotate2);
}

void DemoObject::TextMatrix(const Mat4& _matrix) {
	for(size_t r = 0; r < 4; ++r) {
		for(size_t c = 0; c < 4; ++c) {
			if(c != 0) {
				ImGui::SameLine();
			}
			
			ImGui::Text("%0.3f,", _matrix.m[r][c]);
		}
	}
}

