#pragma once

#include <Windows.h>

/// std
#include <memory>
#include <unordered_map>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"

/// editor
#include "Engine/Editor/Views/EditorWindows/EditorViewCollection.h"
#include "Engine/Editor/Math/ImGuiSceneImageInfo.h"

namespace ONEngine {
/// 前方宣言
class AssetCollection;
class DxManager;
class WindowManager;
class Window;
class EntityComponentSystem;
class SceneManager;
} /// ONEngine


/// ///////////////////////////////////////////////////
/// ImGuiManager
/// ///////////////////////////////////////////////////
namespace Editor {

class EditorManager;

class ImGuiManager final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiManager(ONEngine::DxManager*, ONEngine::WindowManager*, ONEngine::EntityComponentSystem*, EditorManager*, ONEngine::SceneManager*);
	~ImGuiManager();

	/// @brief 初期化
	/// @param _assetCollection AssetCollection 
	void Initialize(ONEngine::AssetCollection* _assetCollection);

	/// @brief 終了処理
	void Finalize();

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
	void UpdateMousePosition(HWND _winHwnd, const ONEngine::Vector2& _renderTargetSize);


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
	ONEngine::DxManager* dxManager_;
	ONEngine::WindowManager* pWindowManager_;
	ONEngine::AssetCollection* pAssetCollection_;
	ONEngine::EntityComponentSystem* pEntityComponentSystem_;
	EditorManager* pEditorManager_;
	ONEngine::SceneManager* pSceneManager_;
	ONEngine::Window* pImGuiWindow_;
	ONEngine::Window* pDebugGameWindow_;

	std::unique_ptr<EditorViewCollection> imGuiWindowCollection_ = nullptr;

	std::unordered_map<std::string, ImGuiSceneImageInfo> sceneImageInfos_ = {}; ///< imguiのシーンイメージ情報

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief imgui windowを設定する
	/// @param _window Window
	void SetImGuiWindow(ONEngine::Window* _window);

	/// @brief game debug windowを取得する
	/// @return　Window
	ONEngine::Window* GetDebugGameWindow() const;

	/// @brief ImageInfoを取得する
	/// @param _name ImageInfoの名前
	/// @return 見つかったImageInfoのポインタ、見つからなかった場合はnullptr
	const ImGuiSceneImageInfo* GetSceneImageInfo(const std::string& _name) const;
};


} /// Editor
