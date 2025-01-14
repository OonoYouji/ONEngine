#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {

	q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	q2 = { 1.0f, 3.0f, 5.0f, 2.0f };
	identity = Quaternion::kIdentity;
	conj = q1.Conjugate();
	inv = q1.Inverse();
	normal = Quaternion::Normalize(q1);
	mul1 = q1 * q2;
	mul2 = q2 * q1;

	norm = q1.Lenght();

}

void DemoObject::Update() {


}

void DemoObject::Debug() {

    auto QuaternionToString = [](const Quaternion& _q) -> std::string {
		return std::format("({:.2f}, {:.2f}, {:.2f}, {:.2f})", _q.x, _q.y, _q.z, _q.w);
    };


	ImGui::Text((QuaternionToString(identity) + ": Identity").c_str());
	ImGui::Text((QuaternionToString(conj) + ": Conjugate").c_str());
	ImGui::Text((QuaternionToString(inv) + ": Inverse").c_str());
	ImGui::Text((QuaternionToString(normal) + ": Normalizes").c_str());
	ImGui::Text((QuaternionToString(mul1) + ": Multiply(q1, q2)").c_str());
	ImGui::Text((QuaternionToString(mul2) + ": Multiply(q2, q1)").c_str());
	ImGui::Text(std::format("{:.2f} : Norm", norm).c_str());

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

