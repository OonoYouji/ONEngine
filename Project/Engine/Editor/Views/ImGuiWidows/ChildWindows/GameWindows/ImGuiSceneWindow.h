#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class ImGuiSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiSceneWindow(ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection, ONEngine::SceneManager* _sceneManager, class ImGuiInspectorWindow* _inspector);
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
	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::AssetCollection*       pAssetCollection_;
	ONEngine::SceneManager*          pSceneManager_;
	class ImGuiInspectorWindow*  pInspector_;

	int manipulateOperation_;
	int manipulateMode_;
};


} /// Editor
