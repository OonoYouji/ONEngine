#pragma once

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


/// ///////////////////////////////////////////////////
/// ImGuiManager
/// ///////////////////////////////////////////////////
class ImGuiManager final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiManager(class DxManager* _dxManager, class WindowManager* _windowManager, class EntityComponentSystem* _pEntityComponentSystem, class EditorManager* _editorManager);
	~ImGuiManager();

	void Initialize(class GraphicsResourceCollection* _graphicsResourceCollection);
	void Update();
	void Draw();

	void AddSceneImageInfo(const std::string& _name, const ImGuiSceneImageInfo& _info) {
		sceneImageInfos_[_name] = _info;
	}

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* dxManager_;
	class WindowManager* windowManager_;
	class GraphicsResourceCollection* resourceCollection_;
	class EntityComponentSystem* pEntityComponentSystem_;
	class EditorManager* pEditorManager_;
	class Window* imGuiWindow_;
	class Window* debugGameWindow_;

	bool isGameDebug_ = false;

	std::unique_ptr<ImGuiWindowCollection> imGuiWindowCollection_ = nullptr;

	std::unordered_map<std::string, ImGuiSceneImageInfo> sceneImageInfos_ = {}; ///< imguiのシーンイメージ情報

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

	const ImGuiSceneImageInfo& GetSceneImageInfo(const std::string& _name) const {
		auto it = sceneImageInfos_.find(_name);
		if (it != sceneImageInfos_.end()) {
			return it->second;
		}

		return {};
	}
};


