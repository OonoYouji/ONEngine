#pragma once

/// std
#include <memory>
#include <map>
#include <functional>

#include "../../Collection/ImGuiWindowCollection.h"

/// //////////////////////////////////////////////////////
/// PrefabのInspector用のWindow
/// //////////////////////////////////////////////////////
class ImGuiPrefabInspectorWindow : public IImGuiChildWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabInspectorWindow(class EntityComponentSystem* _ecs, class EditorManager* _editorManager);
	~ImGuiPrefabInspectorWindow() override = default;

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief inspector に表示する情報のポインタを設定する
	/// @param _pointer 表示したい物のポインタ(整数型)
	void SetSelectedEntity(std::uintptr_t _pointer) { selectedPointer_ = _pointer; }

	template<typename T>
	void RegisterComponent(std::function<void(class IComponent*)> _func);


	void EntityInspector();

	class GameEntity* GetSelectedEntity() const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class EntityComponentSystem* pECS_;
	class EditorManager* pEditorManager_;


	std::uintptr_t selectedPointer_; ///< 選択したポインタ

	std::vector<std::function<void()>> inspectorFunctions_; ///< inspectorに表示する関数のポインタ
	class IComponent* selectedComponent_ = nullptr; ///< 選択したcomponentのポインタ

	std::unordered_map<size_t, std::function<void(class IComponent*)>> componentDebugFuncs_;

	/* ----- add component ----- */

	std::map<size_t, std::string> componentNames_;

};

template<typename T>
inline void ImGuiPrefabInspectorWindow::RegisterComponent(std::function<void(class IComponent*)> _func) {
	/// hash計算
	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}
	size_t hash = std::hash<std::string>()(name);

	componentDebugFuncs_[hash] = _func;
	componentNames_[hash] = name;
}

