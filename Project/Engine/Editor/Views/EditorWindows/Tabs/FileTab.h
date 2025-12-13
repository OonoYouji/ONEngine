#pragma once

/// engine
#include "../EditorViewCollection.h"

/// ///////////////////////////////////////////////////
/// File操作用のImGuiWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class FileTab : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	FileTab();
	~FileTab() {}

};

} /// Editor
