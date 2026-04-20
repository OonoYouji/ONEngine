#pragma once

/// std
#include <string>

/// engine
#include "../../EditorViewCollection.h"

namespace ONEngine::Asset {
class AssetCollection;
}

namespace Editor {

/// ///////////////////////////////////////////////////
/// TextureのPreviewを行う 主にシーンのテクスチャを確認するためのウィンドウ
/// ///////////////////////////////////////////////////
class TexturePreviewWindow : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	TexturePreviewWindow(ONEngine::Asset::AssetCollection* _assetCollection);
	~TexturePreviewWindow();

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::Asset::AssetCollection* pAssetCollection_;
	std::string searchFilter_;

};

} /// Editor
