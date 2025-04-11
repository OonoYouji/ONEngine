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

	ImGuiInspectorWindow();
	~ImGuiInspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

	/// @brief inspector に表示する情報のポインタを設定する
	/// @param _pointer 表示したい物のポインタ(整数型)
	void SetSelectedEntity(std::uintptr_t _pointer) { selectedPointer_ = _pointer; }

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::uintptr_t selectedPointer_; ///< 選択したポインタ

	std::vector<std::function<void()>> inspectorFunctions_; ///< inspectorに表示する関数のポインタ
	class IComponent* selectedComponent_ = nullptr; ///< 選択したcomponentのポインタ

};

