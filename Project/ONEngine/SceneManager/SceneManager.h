#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>
#include <vector>

#include "GraphicManager/SceneLayer/SceneLayer.h"
#include "GraphicManager/RenderTextureManager/RenderTexture.h"

#include "AbstructSceneFactory.h"
#include "GraphicManager/PostEffect/PipelineRegistry/BasePostEffectPipelineRegistry.h"

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


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_sceneFactory"></param>
	void Initialize(AbstructSceneFactory* _sceneFactory, BasePostEffectPipelineRegistry* _pipelineRegstry);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	


	/// <summary>
	/// imguiのデバッグ処理
	/// </summary>
	void ImGuiDebug();



private:

	/// ===================================================
	/// private : methods
	/// ===================================================


	/// <summary>
	/// シーンの読み込み処理
	/// </summary>
	/// <param name="_sceneName"></param>
	void Load(const std::string& _sceneName);



private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<AbstructSceneFactory>                       sceneFactory_;
	std::unique_ptr<BasePostEffectPipelineRegistry>             postEffectPipelineRegistry_;

	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes_;
	std::string                                                 nextSceneName_;
	std::string                                                 currentSceneName_;

	class GameObjectManager*                                    pGameObjectManager_ = nullptr;
	class CollisionManager*                                     pCollisionManager_  = nullptr;

	std::vector<class SceneLayer*>                              sceneLayers_;
	std::unique_ptr<RenderTexture>                              finalRenderTex_;



public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// 次のシーンをセット
	/// </summary>
	/// <param name="_nextSceneName"> : 次のシーンの名前 </param>
	void SetNextScene(const std::string& _nextSceneName);

	/// <summary>
	/// 現在のシーンの名前のゲッタ
	/// </summary>
	/// <returns> : 現在のシーンstring </returns>
	const std::string& GetCurrentScene() const { return currentSceneName_; }

	/// <summary>
	/// シーンレイヤー配列のセット
	/// </summary>
	/// <param name="sceneLayers"> : </param>
	void SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers);

	/// <summary>
	/// シーンレイヤーのゲット
	/// </summary>
	/// <param name="layerId"> : レイヤーのid      </param>
	/// <returns>              : レイヤーのポインタ </returns>
	SceneLayer* GetSceneLayer(uint32_t layerId) const { return sceneLayers_.at(layerId); }


	/// <summary>
	/// RTVの最終結果を返す
	/// </summary>
	/// <returns> : 描画の最終結果</returns>
	RenderTexture* GetFinalRenderTex() const { return finalRenderTex_.get(); }


	/// <summary>
	/// 描画に使われるライトのゲッタ
	/// </summary>
	/// <returns> : 直線ライト </returns>
	class DirectionalLight* GetDirectionalLight();


private:
	
	/// ===================================================
	/// private : delete copy
	/// ===================================================

	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};
