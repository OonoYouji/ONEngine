#pragma once

/// editor
#include "../EditorViewCollection.h"

namespace Editor {

class EditorTab : public IEditorWindowContainer {
public:

	EditorTab();
	~EditorTab() override = default;

};

} /// namespace Editor