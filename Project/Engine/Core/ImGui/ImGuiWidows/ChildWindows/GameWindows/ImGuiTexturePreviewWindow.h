#pragma once

/// std
#include <string>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"


/// ///////////////////////////////////////////////////
/// TextureのPreviewを行う 主にシーンのテクスチャを確認するためのウィンドウ
/// ///////////////////////////////////////////////////
class ImGuiTexturePreviewWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiTexturePreviewWindow(class AssetCollection* _assetCollection);
	~ImGuiTexturePreviewWindow();

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pAssetCollection_;
	std::string searchFilter_;

};

