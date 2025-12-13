#pragma once

/// std
#include <vector>
#include <unordered_map>
#include <string>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"


/// ///////////////////////////////////////////////////
/// ImGuiにGameのログを表示するWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class ImGuiConsoleWindow : public IEditorView {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiConsoleWindow() {}
	~ImGuiConsoleWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::vector<std::string> logs_;
	std::unordered_map<std::string, size_t> indices_;
	std::unordered_map<std::string, size_t> logCounts_;
	

};

} /// Editor
