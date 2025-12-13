#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class DebugSceneView : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DebugSceneView(ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection, ONEngine::SceneManager* _sceneManager, class InspectorWindow* _inspector);
	~DebugSceneView() {}

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
	class InspectorWindow*  pInspector_;

	int manipulateOperation_;
	int manipulateMode_;
};


} /// Editor
