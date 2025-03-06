#pragma once

/// std
#include <functional>
#include <vector>

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

	/// @brief imgui のレンダリング関数を登録する
	/// @param _func レンダリング関数
	void RegisterImguiRenderFunc(std::function<void()> _func) {
		imguiRenderFuncs_.push_back(_func);
	}

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager*     dxManager_     = nullptr;
	class WindowManager* windowManager_ = nullptr;
	class Window*        imguiWindow_   = nullptr;

	std::vector<std::function<void()>> imguiRenderFuncs_;

	bool isGameDebug_ = false;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================


	bool GetIsGameDebug() const { return isGameDebug_; }

};

