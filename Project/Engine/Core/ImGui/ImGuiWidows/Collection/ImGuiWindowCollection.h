#pragma once

/// std
#include <vector>
#include <memory>


/// ///////////////////////////////////////////////////
/// ImGuiWindowCollection
/// ///////////////////////////////////////////////////
class ImGuiWindowCollection {
public:

	ImGuiWindowCollection();
	~ImGuiWindowCollection();

	/// @brief 更新
	void Update();

	/// @brief windowの追加
	void AddWindow(std::unique_ptr<class IImGuiWindow> _window) {
		iImguiWindows_.push_back(std::move(_window));
	}

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<class IImGuiWindow>> iImguiWindows_;

};


/// ///////////////////////////////////////////////////
/// ImGuiWindow: Begin, Endをラップするクラス
/// ///////////////////////////////////////////////////
class IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IImGuiWindow() = default;

	/// @brief imgui windowの描画処理
	virtual void ImGuiFunc() = 0;
};
