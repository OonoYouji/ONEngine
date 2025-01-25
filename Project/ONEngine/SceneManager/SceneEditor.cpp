#include "SceneEditor.h"

/// engine
#include "WindowManager/ConsoleManager.h"
#include "SceneManager.h"


SceneEditor::SceneEditor() {}
SceneEditor::~SceneEditor() {}

void SceneEditor::Initialize() {
	ConsoleManager* consoleManager = ConsoleManager::GetInstance();

	/// consoleに登録
	consoleManager->RegisterFunction([&](ImGuiWindowFlags _windowFlags) { ImGuiDebug(_windowFlags); });

	/// pointerの取得
	pSceneManager_ = SceneManager::GetInstance();
	sceneNames_    = pSceneManager_->GetSceneFactory()->GetSceneNames();

}


void SceneEditor::ImGuiDebug(ImGuiWindowFlags _imguiWindowFlags) {

	if (!ImGui::Begin("SceneEditor", nullptr, _imguiWindowFlags)) {
		ImGui::End();
		return;
	}

	/// シーンの編集を行うためのウィンドウを表示
	SceneChange();

	ImGui::End();
}

void SceneEditor::SceneChange() {
	if (!ImGui::TreeNode("SceneChange")) {
		return;
	}

	

	ImGui::TreePop();
}
