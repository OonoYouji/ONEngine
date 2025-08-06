#pragma once

#include <Windows.h>

/// std
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>

/// external
#include <imgui.h>

/// engine
#include "ImGuiWidows/Collection/ImGuiWindowCollection.h"
#include "Math/ImGuiSceneImageInfo.h"
#include "Engine/Core/Utility/Math/Vector2.h"

/// ///////////////////////////////////////////////////
/// ImGuiManager
/// ///////////////////////////////////////////////////
class ImGuiManager final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiManager(class DxManager*, class WindowManager*, class EntityComponentSystem*, class EditorManager*, class SceneManager*);
	~ImGuiManager();

	void Initialize(class GraphicsResourceCollection* _graphicsResourceCollection);
	void Update();
	void Draw();

	void AddSceneImageInfo(const std::string& _name, const ImGuiSceneImageInfo& _info);

	void UpdateMousePosition(HWND _winHwnd, const Vector2& _renderTargetSize);

	void OutputImGuiStyle(const std::string& _fileName) const;
	void InputImGuiStyle(const std::string& _fileName) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* dxManager_;
	class WindowManager* windowManager_;
	class GraphicsResourceCollection* resourceCollection_;
	class EntityComponentSystem* pEntityComponentSystem_;
	class EditorManager* pEditorManager_;
	class SceneManager* pSceneManager_;
	class Window* imGuiWindow_;
	class Window* debugGameWindow_;

	std::unique_ptr<ImGuiWindowCollection> imGuiWindowCollection_ = nullptr;

	std::unordered_map<std::string, ImGuiSceneImageInfo> sceneImageInfos_ = {}; ///< imguiのシーンイメージ情報

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief imgui windowを設定する
	/// @param _window Window
	void SetImGuiWindow(Window* _window);

	/// @brief game debug windowを取得する
	/// @return　Window
	class Window* GetDebugGameWindow() const;

	const ImGuiSceneImageInfo* GetSceneImageInfo(const std::string& _name) const;
};


