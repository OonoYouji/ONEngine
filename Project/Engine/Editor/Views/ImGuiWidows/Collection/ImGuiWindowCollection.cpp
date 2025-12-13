#include "ImGuiWindowCollection.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../ImGuiMainWindow.h"
#include "../ParentWindows/ImGuiGameWindow.h"
#include "../ParentWindows/ImGuiEditorWindow.h"
#include "../ParentWindows/ImGuiFileWindow.h"

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
	AddParentWindow("File", std::make_unique<ImGuiFileWindow>());
	AddParentWindow("Game", std::make_unique<ImGuiGameWindow>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddParentWindow("Prefab", std::make_unique<ImGuiEditorWindow>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));

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

void EditorViewCollection::AddParentWindow(const std::string& _name, std::unique_ptr<class IEditorViewContainer> _window) {
	parentWindowNames_.push_back(_name);
	_window->pImGuiManager_ = pImGuiManager_;
	for (auto& child : _window->children_) {
		child->pImGuiManager_ = pImGuiManager_;
	}

	parentWindows_.push_back(std::move(_window));
}

void EditorViewCollection::MainMenuUpdate() {
	/// ----- MainMenuの更新(選択されたMenuの内容を別の処理で表示する) ----- ///

	if (!ImGui::BeginMainMenuBar()) {
		return;
	}

	for (int i = 0; auto& name : parentWindowNames_) {
		int save = selectedMenuIndex_;

		if (i == save) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		}

		if (ImGui::Button(name.c_str())) {
			selectedMenuIndex_ = i;
		}

		if (i == save) {
			ImGui::PopStyleColor();
		}

		++i;
	}

	ImGui::EndMainMenuBar();
}




/// ///////////////////////////////////////////////////
/// ImGuiの親windowクラス
/// ///////////////////////////////////////////////////

void IEditorViewContainer::UpdateViews() {
	for (auto& child : children_) {
		child->ShowImGui();
	}
}

IEditorView* IEditorViewContainer::AddView(std::unique_ptr<class IEditorView> _child) {
	class IEditorView* child = _child.get();
	children_.push_back(std::move(_child));
	return child;
}
