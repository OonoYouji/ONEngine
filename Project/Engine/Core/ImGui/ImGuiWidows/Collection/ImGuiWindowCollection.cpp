#include "ImGuiWindowCollection.h"

/// external
#include <imgui.h>

/// engine
#include "../ImGuiMainWindow.h"

#include "../GameWindows/ImGuiGameWindow.h"
#include "../ImGuiEditorWindow.h"
#include "../ImGuiEditorWindow.h"


/// ///////////////////////////////////////////////////
/// ImGuiWindowCollection
/// ///////////////////////////////////////////////////
ImGuiWindowCollection::ImGuiWindowCollection(
	EntityComponentSystem* _pEntityComponentSystem,
	GraphicsResourceCollection* _resourceCollection,
	ImGuiManager* _imGuiManager,
	EditorManager* _editorManager,
	SceneManager* _sceneManager)
	: pImGuiManager_(_imGuiManager) {

	/// ここでwindowを生成する
	AddParentWindow("Game", std::make_unique<ImGuiGameWindow>(_pEntityComponentSystem, _resourceCollection, _editorManager, _sceneManager));
	AddParentWindow("Edit", std::make_unique<ImGuiEditorWindow>());

}

ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {

	MainMenuUpdate();

	parentWindows_[selectedMenuIndex_]->ImGuiFunc();

}

void ImGuiWindowCollection::AddParentWindow(const std::string& _name, std::unique_ptr<class IImGuiParentWindow> _window) {
	parentWindowNames_.push_back(_name);
	_window->pImGuiManager_ = pImGuiManager_;
	for (auto& child : _window->children_) {
		child->pImGuiManager_ = pImGuiManager_;
	}

	parentWindows_.push_back(std::move(_window));
}

void ImGuiWindowCollection::MainMenuUpdate() {
	if (!ImGui::BeginMainMenuBar()) {
		return;
	}

	for (int i = 0; auto & name : parentWindowNames_) {
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

void IImGuiParentWindow::UpdateChildren() {
	for (auto& child : children_) {
		child->ImGuiFunc();
	}
}

IImGuiChildWindow* IImGuiParentWindow::AddChild(std::unique_ptr<class IImGuiChildWindow> _child) {
	class IImGuiChildWindow* child = _child.get();
	children_.push_back(std::move(_child));
	return child;
}
