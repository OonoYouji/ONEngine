#pragma once

/// editor
#include "../../EditorViewCollection.h"

namespace Editor {

class TestWindow : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	TestWindow() = default;
	~TestWindow() override = default;

	void ShowImGui() override;
};

} /// namespace Editor