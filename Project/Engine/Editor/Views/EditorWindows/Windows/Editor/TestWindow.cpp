#include "TestWindow.h"

#include "Engine/Editor/Math/ImGuiMath.h"

using namespace Editor;

void TestWindow::ShowImGui() {
	if(!ImGui::Begin("Test Window")) {
		ImGui::End();
		return;
	}


	static ONEngine::Vector3 vec[3] = {};

	static bool isUnified[3] = {};
	ImMathf::DrawVec3Control("position", vec[0], 1.0f, 100.0f, &isUnified[0]);
	ImMathf::DrawVec3Control("rotate",   vec[1], 1.0f, 100.0f, &isUnified[1]);
	ImMathf::DrawVec3Control("scale",    vec[2], 1.0f, 100.0f, &isUnified[2]);

	ImGui::End();
}
