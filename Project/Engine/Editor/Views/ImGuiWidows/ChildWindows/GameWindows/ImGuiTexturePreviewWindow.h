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
class ImGuiTexturePreviewWindow : public IEditorView {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiTexturePreviewWindow(ONEngine::AssetCollection* _assetCollection);
	~ImGuiTexturePreviewWindow();

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_;
	std::string searchFilter_;

};

} /// Editor
