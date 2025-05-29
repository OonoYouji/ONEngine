#pragma once

/// std
#include <vector>
#include <functional>

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiInspectorWindow
/// ///////////////////////////////////////////////////
class ImGuiInspectorWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiInspectorWindow(class EditorManager* _editorManager);
	~ImGuiInspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

	/// @brief inspector に表示する情報のポインタを設定する
	/// @param _pointer 表示したい物のポインタ(整数型)
	void SetSelectedEntity(std::uintptr_t _pointer) { selectedPointer_ = _pointer; }

	/// @brief componentのデバッグ表示関数を登録する
	/// @param _hash 登録するcomponentのハッシュ値
	/// @param _func デバッグ処理の関数ポインタ
	void RegisterComponentDebugFunc(size_t _hash, std::function<void(class IComponent*)> _func) {
		componentDebugFuncs_.emplace(_hash, _func);
	}

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class EditorManager* pEditorManager_;

	std::uintptr_t selectedPointer_; ///< 選択したポインタ

	std::vector<std::function<void()>> inspectorFunctions_; ///< inspectorに表示する関数のポインタ
	class IComponent* selectedComponent_ = nullptr; ///< 選択したcomponentのポインタ

	std::unordered_map<size_t, std::function<void(class IComponent*)>> componentDebugFuncs_;
};

