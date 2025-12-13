#include "EditorViewCollection.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Tabs/DevelopTab.h"
#include "Tabs/PrefabTab.h"
#include "Tabs/EditorTab.h"

using namespace Editor;

/// ///////////////////////////////////////////////////
/// ImGuiWindowCollection
/// ///////////////////////////////////////////////////
EditorViewCollection::EditorViewCollection(
	ONEngine::DxManager* _dxm,
	ONEngine::EntityComponentSystem* _ecs,
	ONEngine::AssetCollection* _assetCollection,
	ImGuiManager* _imGuiManager,
	EditorManager* _editorManager,
	ONEngine::SceneManager* _sceneManager)
	: pImGuiManager_(_imGuiManager) {

	/// ここでwindowを生成する
	AddViewContainer("Develop", std::make_unique<DevelopTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddViewContainer("Prefab", std::make_unique<PrefabTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddViewContainer("Editor", std::make_unique<EditorTab>());

	// game windowで開始
	selectedMenuIndex_ = 1;
}

EditorViewCollection::~EditorViewCollection() {}

void EditorViewCollection::Update() {

	MainMenuUpdate();

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

	ImGuiID dockspaceID = ImGui::GetID(windowName_.c_str());
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
