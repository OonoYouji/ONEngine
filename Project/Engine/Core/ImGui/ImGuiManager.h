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

	/// @brief 初期化
	/// @param _assetCollection AssetCollection 
	void Initialize(class AssetCollection* _assetCollection);

	/// @brief 更新
	void Update();

	/// @brief 描画
	void Draw();


	/// @brief SceneImageInfoを追加する
	/// @param _name SceneImageInfoの名前
	/// @param _info 追加するSceneImageInfo
	void AddSceneImageInfo(const std::string& _name, const ImGuiSceneImageInfo& _info);

	/// @brief ImGuiのマウス位置を更新する
	/// @param _winHwnd ImGuiを描画しているWindowのHWND
	/// @param _renderTargetSize RenderTargetのサイズ(px)
	void UpdateMousePosition(HWND _winHwnd, const Vector2& _renderTargetSize);


	/// @brief ImGuiのスタイルを出力する
	/// @param _fileName ファイル名
	void OutputImGuiStyle(const std::string& _fileName) const;

	/// @brief ImGuiのスタイルを入力する
	/// @param _fileName ファイル名
	void InputImGuiStyle(const std::string& _fileName) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	class DxManager* dxManager_;
	class WindowManager* pWindowManager_;
	class AssetCollection* pAssetCollection_;
	class EntityComponentSystem* pEntityComponentSystem_;
	class EditorManager* pEditorManager_;
	class SceneManager* pSceneManager_;
	class Window* pImGuiWindow_;
	class Window* pDebugGameWindow_;

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

	/// @brief ImageInfoを取得する
	/// @param _name ImageInfoの名前
	/// @return 見つかったImageInfoのポインタ、見つからなかった場合はnullptr
	const ImGuiSceneImageInfo* GetSceneImageInfo(const std::string& _name) const;
};


