#include "EventTab.h"

/// editor
#include "../Windows/Develop/GameEventEditorWindow.h"

using namespace Editor;

EventTab::EventTab()
	: IEditorWindowContainer("Event") {

	AddView(std::make_unique<GameEventEditorWindow>("Game Event Editor"));
}
