#pragma once

/// std
#include <string>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

namespace ONEngine {
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

	TexturePreviewWindow(ONEngine::AssetCollection* _assetCollection);
	~TexturePreviewWindow();

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_;
	std::string searchFilter_;

};

} /// Editor
