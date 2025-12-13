#include "GameTab.h"

/// editor
#include "../Windows/Develop/GameSceneView.h"

using namespace Editor;

GameTab::GameTab(ONEngine::AssetCollection* _ac)
	: IEditorWindowContainer("Game") {

	AddView(std::make_unique<GameSceneView>(_ac, "GameView##GameSceneView"));
}
