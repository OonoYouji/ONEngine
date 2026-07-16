#pragma once

/// std
#include <vector>
#include <unordered_map>
#include <string>

/// engine
#include "../../EditorViewCollection.h"


/// ///////////////////////////////////////////////////
/// ImGuiにGameのログを表示するWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class ConsoleWindow : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ConsoleWindow() {}
	~ConsoleWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	bool showInfo_ = true;
	bool showWarning_ = true;
	bool showError_ = true;

	bool showEngine_ = true;
	bool showScriptEngine_ = true;
	bool showApplication_ = true;

	bool autoScroll_ = true;

};

} /// Editor
