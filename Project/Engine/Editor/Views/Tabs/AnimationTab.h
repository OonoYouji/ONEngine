#pragma once

/// engine
#include "../EditorViewCollection.h"

namespace Editor {

/// ///////////////////////////////////////////////////
/// アニメーション編集用タブ
/// ///////////////////////////////////////////////////
class AnimationTab : public IEditorWindowContainer {
public:
    AnimationTab(class ONEngine::Asset::AssetCollection* _assetCollection);
    ~AnimationTab() override = default;
};

} /// namespace Editor
