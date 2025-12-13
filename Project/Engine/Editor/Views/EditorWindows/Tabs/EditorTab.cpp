#include "EditorTab.h"

/// editor
#include "../Windows/Editor/TestWindow.h"

using namespace Editor;

EditorTab::EditorTab() : IEditorWindowContainer("Editor") {
	AddView(std::make_unique<TestWindow>());
}
