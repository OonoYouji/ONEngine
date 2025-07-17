#pragma once

/// std
#include <vector>
#include <unordered_map>
#include <string>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"


/// ///////////////////////////////////////////////////
/// ImGuiGameWindow
/// ///////////////////////////////////////////////////
class ImGuiConsoleWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiConsoleWindow() {}
	~ImGuiConsoleWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::vector<std::string> logs_;
	std::unordered_map<std::string, size_t> indices_;
	std::unordered_map<std::string, size_t> logCounts_;
	

};

