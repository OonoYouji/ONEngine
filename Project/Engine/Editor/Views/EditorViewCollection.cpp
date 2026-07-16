#include "EditorViewCollection.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Editor/Manager/ImGuiManager.h"
#include "Tabs/DevelopTab.h"
#include "Tabs/GameTab.h"
#include "Tabs/PrefabTab.h"
#include "Tabs/EditorTab.h"
#include "Tabs/AITab.h"
#include "Tabs/EventTab.h"
#include "Tabs/AnimationTab.h"
#include "Engine/Core/Event/GameEventData.h"

using namespace Editor;

/// ///////////////////////////////////////////////////
/// ImGuiWindowCollection
/// ///////////////////////////////////////////////////
EditorViewCollection::EditorViewCollection(
	ONEngine::DxManager* _dxm,
	ONEngine::EntityComponentSystem* _ecs,
	ONEngine::Asset::AssetCollection* _assetCollection,
	ImGuiManager* _imGuiManager,
	EditorManager* _editorManager,
	ONEngine::SceneManager* _sceneManager)
	: pImGuiManager_(_imGuiManager), pSceneManager_(_sceneManager) {

	/// ここでwindowを生成する
	AddViewContainer("Develop", std::make_unique<DevelopTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddViewContainer("Game", std::make_unique<GameTab>(_assetCollection));
	AddViewContainer("Prefab", std::make_unique<PrefabTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddViewContainer("Editor", std::make_unique<EditorTab>());
	AddViewContainer("AI", std::make_unique<AITab>(_dxm, _ecs, _editorManager, _sceneManager));
	AddViewContainer("Event", std::make_unique<EventTab>());
	AddViewContainer("Animation", std::make_unique<AnimationTab>(_assetCollection));

	// game windowで開始
	selectedMenuIndex_ = 0;
}

EditorViewCollection::~EditorViewCollection() {}

void EditorViewCollection::Update() {

	MainMenuUpdate();

	/// 終了リクエストの確認
	if(pImGuiManager_->GetWindowManager()->IsCloseRequested()) {
		if(pSceneManager_->IsDirty() || ONEngine::GameEventManager::GetInstance().IsDirty()) {
			ImGui::OpenPopup("Save Changes?");
		} else {
			PostQuitMessage(0);
		}
	}

	if(ImGui::BeginPopupModal("Save Changes?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::string msg = "The following have unsaved changes:\n";
		if (pSceneManager_->IsDirty()) msg += " - Scene\n";
		if (ONEngine::GameEventManager::GetInstance().IsDirty()) msg += " - Game Events\n";
		msg += "\nSave changes before exiting?";

		ImGui::Text(msg.c_str());
		ImGui::Separator();

		if(ImGui::Button("Save", ImVec2(120, 0))) {
			if (pSceneManager_->IsDirty()) pSceneManager_->SaveCurrentScene();
			if (ONEngine::GameEventManager::GetInstance().IsDirty()) ONEngine::GameEventManager::GetInstance().Save();
			
			PostQuitMessage(0);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if(ImGui::Button("Don't Save", ImVec2(120, 0))) {
			PostQuitMessage(0);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel", ImVec2(120, 0))) {
			pImGuiManager_->GetWindowManager()->SetCloseRequested(false);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	/// 選択されたMenuの内容を表示する
	parentWindows_[selectedMenuIndex_]->ShowImGui();
	ONEngine::DebugConfig::selectedMode_ = selectedMenuIndex_;

}

void EditorViewCollection::AddViewContainer(const std::string& _name, std::unique_ptr<class IEditorWindowContainer> _window) {
	parentWindowNames_.push_back(_name);
	_window->pImGuiManager_ = pImGuiManager_;
	for(auto& child : _window->children_) {
		child->pImGuiManager_ = pImGuiManager_;
	}

	parentWindows_.push_back(std::move(_window));
}

void EditorViewCollection::MainMenuUpdate() {
	/// ----- MainMenuの更新(選択されたMenuの内容を別の処理で表示する) ----- ///

	if(!ImGui::BeginMainMenuBar()) {
		return;
	}

	for(int i = 0; auto& name : parentWindowNames_) {
		int save = selectedMenuIndex_;

		if(i == save) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		}

		if(ImGui::Button(name.c_str())) {
			selectedMenuIndex_ = i;
		}

		if(i == save) {
			ImGui::PopStyleColor();
		}

		++i;
	}

	ImGui::EndMainMenuBar();
}




/// ///////////////////////////////////////////////////
/// ImGuiの親windowクラス
/// ///////////////////////////////////////////////////

Editor::IEditorWindowContainer::IEditorWindowContainer(const std::string& _windowName)
	: windowName_(_windowName) {
}

void Editor::IEditorWindowContainer::ShowImGui() {
	uint32_t imGuiFlags_ = 0;
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(ONEngine::EngineConfig::kWindowSize.x, ONEngine::EngineConfig::kWindowSize.y));
	if(!ImGui::Begin(windowName_.c_str(), nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID((windowName_ + "DockSpace").c_str());
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateViews();

	ImGui::End();
}

void IEditorWindowContainer::UpdateViews() {
	for(auto& child : children_) {
		child->ShowImGui();
	}
}

IEditorWindow* IEditorWindowContainer::AddView(std::unique_ptr<class IEditorWindow> _child) {
	class IEditorWindow* child = _child.get();
	children_.push_back(std::move(_child));
	return child;
}
