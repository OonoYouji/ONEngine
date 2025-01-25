#pragma once

#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <memory>
#include <string>
#include <vector>

/// externals
#include <imgui.h>

#include "GraphicManager/SceneLayer/SceneLayer.h"
#include "GraphicManager/RenderTextureManager/RenderTexture.h"
#include "GraphicManager/Light/LightGroup.h"

#include "AbstructSceneFactory.h"
#include "SceneEditor.h"

class BaseScene;

using namespace Microsoft::WRL;

/// ===================================================
/// シーンの管理クラス
/// ===================================================
class SceneManager final {
	SceneManager() = default;
	~SceneManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// インスタンス確保
	/// </summary>
	static SceneManager* GetInstance();

	/// @brief 初期化関数
	/// @param _sceneFactory シーンの生成クラス
	void Initialize(AbstructSceneFactory* _sceneFactory);

	/// @brief 終了処理
	void Finalize();

	/// @brief シーンの更新
	void Update();

	/// @brief シーンの描画
	void Draw();
	
	/// @brief imguiでのデバッグ
	void ImGuiDebug();

private:

	/// @brief シーンの読み込み処理
	/// @param _sceneName 
	void Load(const std::string& _sceneName);

	/// @brief シーンをコンソールに表示する
	/// @param _imguiWindowFlags 
	void Scene(ImGuiWindowFlags _imguiWindowFlags);

	/// @brief デバッグシーンをコンソールに表示する
	/// @param _imguiWindowFlags 
	void DebugScene(ImGuiWindowFlags _imguiWindowFlags);

	/// @brief ゲームシーンを出したり、戻したりする
	void PlayControl();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<AbstructSceneFactory>                       sceneFactory_;
	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes_;
	std::string nextSceneName_;
	std::string currentSceneName_;

	class GameObjectManager* pGameObjectManager_ = nullptr;
	class CollisionManager*  pCollisionManager_  = nullptr;

	std::vector<class SceneLayer*> sceneLayers_;
	std::unique_ptr<RenderTexture> finalRenderTex_;

	std::unique_ptr<LightGroup> lightGroup_;

	Vec2 sceneRectMin_, sceneRectMax_;
	std::unique_ptr<SceneEditor> sceneEditor_;

public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief 次のシーンをセット
	/// @param _nextSceneName 次のシーンの名前
	void SetNextScene(const std::string& _nextSceneName);

	/// @brief 現在のシーンの名前を取得
	const std::string& GetCurrentScene() const { return currentSceneName_; }

	/// @brief シーンのレイヤーをセット
	/// @param sceneLayers シーンのレイヤー配列
	void SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers);

	/// @brief シーンのレイヤーを取得する
	/// @param layerId 取得したいレイヤーのID
	/// @return シーンのレイヤー
	SceneLayer* GetSceneLayer(uint32_t layerId) const { return sceneLayers_.at(layerId); }

	/// @brief 描画の最終結果を取得
	/// @return 描画の最終結果RTV
	RenderTexture* GetFinalRenderTex() const { return finalRenderTex_.get(); }

	/// @brief ライトのグループを取得
	/// @return ライトグループ
	LightGroup* GetLightGroup() { return lightGroup_.get(); }

	/// @brief シーンの矩形の最小値を取得
	const Vec2& GetSceneRectMin() const { return sceneRectMin_; }

	/// @brief シーンの矩形の最大値を取得
	const Vec2& GetSceneRectMax() const { return sceneRectMax_; }

	/// @brief シーンの生成するクラスを取得
	AbstructSceneFactory* GetSceneFactory() { return sceneFactory_.get(); }

private:
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};
