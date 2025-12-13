#include "FileTab.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../Windows/File/ImGuiStyleWindow.h"

using namespace Editor;

FileTab::FileTab() : IEditorWindowContainer("File") {
	/// 子ウィンドウの追加
	AddView(std::make_unique<ImGuiStyleWindow>());
}