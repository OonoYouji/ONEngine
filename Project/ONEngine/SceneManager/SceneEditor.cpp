#include "SceneEditor.h"

/// engine
#include "WindowManager/ConsoleManager.h"


SceneEditor::SceneEditor() {}
SceneEditor::~SceneEditor() {}

void SceneEditor::Initialize() {
	ConsoleManager* consoleManager = ConsoleManager::GetInstance();

	/// consoleに登録
	consoleManager->RegisterFunction([&](ImGuiWindowFlags _windowFlags) { ImGuiDebug(_windowFlags); });

}


void SceneEditor::ImGuiDebug(ImGuiWindowFlags _imguiWindowFlags) {

	if (ImGui::Begin("SceneEditor", nullptr, _imguiWindowFlags)) {
		ImGui::End();
		return;
	}

	/// シーンの編集を行うためのウィンドウを表示


	ImGui::End();
}
