#pragma once

/// std
#include <functional>
#include <vector>
#include <memory>

/// external
#include <imgui.h>

/// engine
#include "ImGuiWidows/Collection/ImGuiWindowCollection.h"


/// ///////////////////////////////////////////////////
/// ImGuiManager
/// ///////////////////////////////////////////////////
class ImGuiManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiManager(class DxManager* _dxManager, class WindowManager* _windowManager, class EntityCollection* _entityCollection);
	~ImGuiManager();

	void Initialize(class GraphicsResourceCollection* _graphicsResourceCollection);
	void Update();
	void Draw();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager*                  dxManager_          = nullptr;
	class WindowManager*              windowManager_      = nullptr;
	class GraphicsResourceCollection* resourceCollection_ = nullptr;
	class EntityCollection*           entityCollection_   = nullptr;
	class Window*                     imGuiWindow_        = nullptr;
	class Window*                     debugGameWindow_    = nullptr;

	bool isGameDebug_ = false;

	ImTextureID startImage_;
	ImTextureID endImage_;

	std::unique_ptr<ImGuiWindowCollection> imGuiWindowCollection_ = nullptr;

public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief imgui windowを設定する
	/// @param _window Window
	void SetImGuiWindow(Window* _window) { imGuiWindow_ = _window; }



	/// @brief gameをdebugするかのフラグ 
	/// @return 
	bool GetIsGameDebug() const { return isGameDebug_; }

	/// @brief game debug windowを取得する
	/// @return　Window
	class Window* GetDebugGameWindow() const { return debugGameWindow_; }

};


