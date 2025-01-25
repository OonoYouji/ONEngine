#pragma once

/// std
#include <vector>
#include <string>

/// external
#include <imgui.h>


/// ======================================
/// シーンの編集を行うクラス
/// ======================================
class SceneEditor final {
public:

	/// ======================================
	/// public : methods
	/// ======================================

	SceneEditor();
	~SceneEditor();

	/// @brief 初期化処理
	void Initialize();

	/// @brief imguiでのデバッグ
	/// @param _imguiWindowFlags 
	void ImGuiDebug(ImGuiWindowFlags _imguiWindowFlags);


private:
	
	/// ======================================
	/// private : objects
	/// ======================================

	void SceneChange();



private:

	/// ======================================
	/// private : objects
	/// ======================================

	class SceneManager*      pSceneManager_ = nullptr;
	std::vector<std::string> sceneNames_;

};

