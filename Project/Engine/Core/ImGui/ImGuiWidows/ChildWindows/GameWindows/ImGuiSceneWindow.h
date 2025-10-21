#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
class ImGuiSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	ImGuiSceneWindow(class EntityComponentSystem* _ecs, class AssetCollection* _grc, class SceneManager* _sceneManager, class ImGuiInspectorWindow* _inspector);
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

	class EntityComponentSystem* pEcs_;
	class AssetCollection* pGrc_ = nullptr;
	class SceneManager* pSceneManager_ = nullptr;
	class ImGuiInspectorWindow* pInspector_ = nullptr;

	int manipulateOperation_;
	int manipulateMode_;
};

