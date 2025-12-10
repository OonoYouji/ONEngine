﻿#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
namespace ONEngine {

class ImGuiSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiSceneWindow(class EntityComponentSystem* _ecs, class AssetCollection* _assetCollection, class SceneManager* _sceneManager, class ImGuiInspectorWindow* _inspector);
	~ImGuiSceneWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief GamePlayモードの設定
	/// @param _isGamePlay Runtime中かどうか
	void SetGamePlay(bool _isGamePlay);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// --------------- other class pointers --------------- ///
	class EntityComponentSystem* pEcs_;
	class AssetCollection*       pAssetCollection_;
	class SceneManager*          pSceneManager_;
	class ImGuiInspectorWindow*  pInspector_;

	int manipulateOperation_;
	int manipulateMode_;
};


} /// ONEngine
