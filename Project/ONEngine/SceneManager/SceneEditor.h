#pragma once

/// external
#include <imgui.h>

/// ======================================
/// シーンの編集を行うクラス
/// ======================================
class SceneEditor final {
public:

	SceneEditor();
	~SceneEditor();

	/// @brief 初期化処理
	void Initialize();

	/// @brief imguiでのデバッグ
	/// @param _imguiWindowFlags 
	void ImGuiDebug(ImGuiWindowFlags _imguiWindowFlags);


private:



};

