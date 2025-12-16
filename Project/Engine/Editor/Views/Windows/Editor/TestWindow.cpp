#include "TestWindow.h"

#define NOMINMAX

#include "Engine/Editor/EditorUtils.h"

using namespace Editor;

void TestWindow::ShowImGui() {
	if(!ImGui::Begin("Test Window")) {
		ImGui::End();
		return;
	}


	constexpr float minValue = std::numeric_limits<float>::lowest();
	constexpr float maxValue = std::numeric_limits<float>::max();
	constexpr int intMinValue = std::numeric_limits<int>::min();
	constexpr int intMaxValue = std::numeric_limits<int>::max();


	static bool unified[3] = {};
	static ONEngine::Vector2 vec2 = { 0.0f, 0.0f };
	DrawVec2Control("vector2", vec2, 0.1f, minValue, maxValue, 100.0f, &unified[0]);

	static ONEngine::Vector3 vec3 = { 0.0f, 0.0f, 0.0f };
	DrawVec3Control("vector3", vec3, 0.1f, minValue, maxValue, 100.0f, &unified[1]);

	static ONEngine::Vector4 vec4 = { 0.0f, 0.0f, 0.0f, 0.0f };
	DrawVec4Control("vector4", vec4, 0.1f, minValue, maxValue, 100.0f, &unified[2]);

	ImGui::Separator();

	static ONEngine::Vector2Int vec2i = { 0, 0 };
	DrawVec2IntControl("vector2i", vec2i, 1.0f, intMinValue, intMaxValue, 100.0f);

	static ONEngine::Vector3Int vec3i = { 0, 0, 0 };
	DrawVec3IntControl("vector3i", vec3i, 1.0f, intMinValue, intMaxValue, 100.0f);

	static ONEngine::Vector4Int vec4i = { 0, 0, 0, 0 };
	DrawVec4IntControl("vector4i", vec4i, 1.0f, intMinValue, intMaxValue, 100.0f);

		
	ImGui::SeparatorText("Int");

	static int intValue = 0;
	Editor::DragInt("Drag Int", intValue, 1.0f, -100, 100);
	Editor::SliderInt("Slider Int", intValue, -100, 100);
	Editor::InputInt("Input Int", intValue, 1, 100);


	ImGui::SeparatorText("Float");

	static float floatValue = 0.0f;
	Editor::DragFloat("Drag Float", floatValue, 0.1f, -100.0f, 100.0f);
	Editor::SliderFloat("Slider Float", floatValue, -100.0f, 100.0f);
	Editor::InputFloat("Input Float", floatValue, 0.1f, 1.0f);



	ImGui::End();
}
