#include "SceneEditor.h"

/// std
#include <algorithm>

/// engine
#include "WindowManager/ConsoleManager.h"
#include "SceneManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "Input/Input.h"


SceneEditor::SceneEditor() {}
SceneEditor::~SceneEditor() {}

void SceneEditor::Initialize() {
	TextureManager::GetInstance()->Load("Scene", "Scene.png");

	ConsoleManager* consoleManager = ConsoleManager::GetInstance();

	/// consoleに登録
	consoleManager->RegisterFunction([&](ImGuiWindowFlags _windowFlags) { ImGuiDebug(_windowFlags); });

	/// pointerの取得
	pSceneManager_ = SceneManager::GetInstance();
	sceneNames_    = pSceneManager_->GetSceneFactory()->GetSceneNames();


	buttonSize_  = ImVec2(64, 64);
	buttonScale_ = 1.0f;
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


	/// ボタンの大きさを変更
	if (Input::PressKey(KeyCode::LControl)) {
		float mouseScroll = Input::MouseScroll();

		if (mouseScroll > 0.0f) {
			buttonScale_ += 0.1F;
		} else if (mouseScroll < 0.0f) {
			buttonScale_ -= 0.1F;
		}

		buttonScale_ = std::clamp(buttonScale_, 0.1f, 10.0f);
	}


	/// シーンのボタンを表示
	TextureManager* textureManager = TextureManager::GetInstance();
	for (const auto& sceneName : sceneNames_) {

		{	/// ボタンの表示
			ImTextureID myTextureID = ImTextureID(textureManager->GetTexture("Scene").GetGPUHandle().ptr); // ここにテクスチャIDを設定

			ImGui::BeginGroup();
			if (ImGui::ImageButton(sceneName.c_str(), myTextureID, ImVec2(buttonSize_.x * buttonScale_, buttonSize_.y * buttonScale_))) {
				pSceneManager_->SetNextScene(sceneName);
			}

			size_t&& maxNameSize = 5 * static_cast<size_t>(buttonScale_ * buttonScale_);
			if (sceneName.size() > maxNameSize) {
				std::string&& clampedSceneName = sceneName.substr(0, maxNameSize) + "...";
				ImGui::Text("%s", clampedSceneName.c_str());
			} else {
				ImGui::Text("%s", sceneName.c_str());
			}

			ImGui::EndGroup();
		}

		ImGui::SameLine();
	}

	ImGui::TreePop();
}
