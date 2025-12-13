#include "TestWindow.h"

#define NOMINMAX

#include "Engine/Editor/EditorUtils.h"

using namespace Editor;

void TestWindow::ShowImGui() {
	if(!ImGui::Begin("Test Window")) {
		ImGui::End();
		return;
	}


	//static ONEngine::Vector3 vec[3] = {};

	//ImMathf::DrawVec3Control("position", vec[0], 1.0f, 100.0f, &isUnified[0]);
	//ImMathf::DrawVec3Control("rotate",   vec[1], 1.0f, 100.0f, &isUnified[1]);
	//ImMathf::DrawVec3Control("scale",    vec[2], 1.0f, 100.0f, &isUnified[2]);

	constexpr float minValue = std::numeric_limits<float>::lowest();
	constexpr float maxValue = std::numeric_limits<float>::max();
	constexpr int intMinValue = std::numeric_limits<int>::min();
	constexpr int intMaxValue = std::numeric_limits<int>::max();


	static bool unified[3] = {};
	static ONEngine::Vector2 vec2 = { 0.0f, 0.0f };
	ImMathf::DrawVec2Control("vector2", vec2, 0.1f, minValue, maxValue, 100.0f, &unified[0]);

	static ONEngine::Vector3 vec3 = { 0.0f, 0.0f, 0.0f };
	ImMathf::DrawVec3Control("vector3", vec3, 0.1f, minValue, maxValue, 100.0f, &unified[1]);

	static ONEngine::Vector4 vec4 = { 0.0f, 0.0f, 0.0f, 0.0f };
	ImMathf::DrawVec4Control("vector4", vec4, 0.1f, minValue, maxValue, 100.0f, &unified[2]);

	ImGui::Separator();

	static ONEngine::Vector2Int vec2i = { 0, 0 };
	ImMathf::DrawVec2IntControl("vector2i", vec2i, 1.0f, intMinValue, intMaxValue, 100.0f);

	static ONEngine::Vector3Int vec3i = { 0, 0, 0 };
	ImMathf::DrawVec3IntControl("vector3i", vec3i, 1.0f, intMinValue, intMaxValue, 100.0f);

	static ONEngine::Vector4Int vec4i = { 0, 0, 0, 0 };
	ImMathf::DrawVec4IntControl("vector4i", vec4i, 1.0f, intMinValue, intMaxValue, 100.0f);


	ImGui::End();
}
