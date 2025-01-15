#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {

	rotation = Quaternion::MakeFromAxis(Vec3(1.0f, 0.4f, -0.2f).Normalize(), 0.45f);
	pointY = { 2.1f, -0.9f, 1.3f };
	rotateMatrix = Quaternion::MakeRotateAxisAngle(Vec3(1.0f, 0.4f, -0.2f).Normalize(), 0.45f);
	rotateByQuaternion = Quaternion::Transform(pointY, rotation);
	rotateByMatrix = Mat4::Transform(pointY, rotateMatrix);

}

void DemoObject::Update() {


}

void DemoObject::Debug() {

	auto QuaternionToString = [](const Quaternion& _q) -> std::string {
		return std::format("({:.2f}, {:.2f}, {:.2f}, {:.2f})", _q.x, _q.y, _q.z, _q.w);
	};


	ImGui::Text((QuaternionToString(rotation) + ": rotation").c_str());

	/// 行列の表示
	ImGui::Text("rotateMatrix");
	TextMatrix(rotateMatrix);

	ImGui::Text(std::format("{:.2f}, {:.2f}, {:.2f} : rotateByQuaternion", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z).c_str());
	ImGui::Text(std::format("{:.2f}, {:.2f}, {:.2f} : rotateByMatrix", rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z).c_str());


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

