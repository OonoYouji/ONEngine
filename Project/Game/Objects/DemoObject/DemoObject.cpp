#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {

	rotation0 = Quaternion::MakeFromAxis({0.71f, 0.71f, 0.0f}, 0.3f);
	rotation1 = Quaternion::MakeFromAxis({0.71f, 0.0f, 0.71f}, 3.141592f);

	interpolate0 = Quaternion::Slerp(rotation0, rotation1, 0.0f);
	interpolate1 = Quaternion::Slerp(rotation0, rotation1, 0.3f);
	interpolate2 = Quaternion::Slerp(rotation0, rotation1, 0.5f);
	interpolate3 = Quaternion::Slerp(rotation0, rotation1, 0.7f);
	interpolate4 = Quaternion::Slerp(rotation0, rotation1, 1.0f);
}

void DemoObject::Update() {

	interpolate2 = Quaternion::Slerp(rotation0, rotation1, 0.5f);

}

void DemoObject::Debug() {

	static bool isAllDigitDraw = false;
	ImGui::Checkbox("all digit draw", &isAllDigitDraw);


	auto QuaternionToString = [&](const Quaternion& _q) -> std::string {
		if(isAllDigitDraw) {
			return std::format("({}, {}, {}, {})", _q.x, _q.y, _q.z, _q.w);
		}
		return std::format("({:.2f}, {:.2f}, {:.2f}, {:.2f})", _q.x, _q.y, _q.z, _q.w);
	};

	ImGui::Text((QuaternionToString(interpolate0) + ": interpolate0, Slerp(q1, q2, 0.0f) ").c_str());
	ImGui::Text((QuaternionToString(interpolate1) + ": interpolate1, Slerp(q1, q2, 0.3f) ").c_str());
	ImGui::Text((QuaternionToString(interpolate2) + ": interpolate2, Slerp(q1, q2, 0.5f) ").c_str());
	ImGui::Text((QuaternionToString(interpolate3) + ": interpolate3, Slerp(q1, q2, 0.7f) ").c_str());
	ImGui::Text((QuaternionToString(interpolate4) + ": interpolate4, Slerp(q1, q2, 1.0f) ").c_str());

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

