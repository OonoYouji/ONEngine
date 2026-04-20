#pragma once

#include "../EditorViewCollection.h"

namespace Editor {

class GameTab : public IEditorWindowContainer {
public:
	GameTab(ONEngine::Asset::AssetCollection* _ac);
	~GameTab() override = default;
};

} /// namespace Editor