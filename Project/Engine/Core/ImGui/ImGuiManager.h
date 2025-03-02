#pragma once

/// ///////////////////////////////////////////////////
/// ImGuiManager
/// ///////////////////////////////////////////////////
class ImGuiManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiManager(class DxManager* _dxManager, class WindowManager* _windowManager);
	~ImGuiManager();

	void Initialize();
	void Update();
	void Draw();


private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief imgui のメインウィンドウを描画する
	void RenderingMainWindow();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager*     dxManager_     = nullptr;
	class WindowManager* windowManager_ = nullptr;
	class Window*        imguiWindow_   = nullptr;

};

