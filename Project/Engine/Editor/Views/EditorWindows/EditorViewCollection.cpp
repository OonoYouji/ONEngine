#include "EditorViewCollection.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Tabs/GameTab.h"
#include "Tabs/PrefabTab.h"
#include "Tabs/FileTab.h"

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
	AddViewContainer("File", std::make_unique<FileTab>());
	AddViewContainer("Game", std::make_unique<GameTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));
	AddViewContainer("Prefab", std::make_unique<PrefabTab>(_dxm, _ecs, _assetCollection, _editorManager, _sceneManager));

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

void IEditorWindowContainer::UpdateViews() {
	for (auto& child : children_) {
		child->ShowImGui();
	}
}

IEditorWindow* IEditorWindowContainer::AddView(std::unique_ptr<class IEditorWindow> _child) {
	class IEditorWindow* child = _child.get();
	children_.push_back(std::move(_child));
	return child;
}
