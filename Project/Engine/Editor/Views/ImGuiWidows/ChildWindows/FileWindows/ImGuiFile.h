﻿#pragma once

#include "../../Collection/ImGuiWindowCollection.h"

/// //////////////////////////////////////////////////////
/// ImGuiのスタイル操作Window
/// //////////////////////////////////////////////////////
namespace ONEngine {

class ImGuiFile : public IImGuiChildWindow {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ImGuiFile();
	~ImGuiFile() {}

	void ShowImGui() override;

};


} /// ONEngine
