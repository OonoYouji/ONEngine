#include "AnimationTab.h"
#include "../Windows/Develop/AnimationEditorWindow.h"

using namespace Editor;

AnimationTab::AnimationTab(ONEngine::Asset::AssetCollection* /*_assetCollection*/)
    : IEditorWindowContainer("Animation") {
    AddView(std::make_unique<AnimationEditorWindow>());
}
