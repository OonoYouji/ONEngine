#pragma once

#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <memory>
#include <string>
#include <vector>

/// extrenals
#include <imgui.h>

#include "GraphicManager/SceneLayer/SceneLayer.h"
#include "GraphicManager/RenderTextureManager/RenderTexture.h"

#include "AbstructSceneFactory.h"

class BaseScene;

using namespace Microsoft::WRL;

/// ===================================================
/// シーンの管理クラス
/// ===================================================
class SceneManager final {
	SceneManager() = default;
	~SceneManager() = default;
public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	static SceneManager* GetInstance();

	void Initialize(AbstructSceneFactory* _sceneFactory);
	void Finalize();


	void Update();
	void Draw();
	

	void ImGuiDebug();


	void SetNextScene(const std::string& _nextSceneName);

	const std::string& GetCurrentScene() const { return currentSceneName_; }

	void SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers);

	SceneLayer* GetSceneLayer(uint32_t layerId) const { return sceneLayers_.at(layerId); }

	RenderTexture* GetFinalRenderTex() const { return finalRenderTex_.get(); }

	class DirectionalLight* GetDirectionalLight();

private:

	void Load(const std::string& _sceneName);

	/// <summary>
	/// シーンをコンソールにデバッグ表示する
	/// </summary>
	/// <param name="_imguiWindowFlags"></param>
	void Scene(ImGuiWindowFlags _imguiWindowFlags);

	/// <summary>
	/// デバッグシーンをコンソールに表示する
	/// </summary>
	/// <param name="_imguiWindowFlags"></param>
	void DebugScene(ImGuiWindowFlags _imguiWindowFlags);

	/// <summary>
	/// sub windowを出したり消したりする
	/// </summary>
	void PlayControl();

private:

	std::unique_ptr<AbstructSceneFactory> sceneFactory_;
	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes_;
	std::string nextSceneName_;
	std::string currentSceneName_;

	class GameObjectManager* pGameObjectManager_ = nullptr;
	class CollisionManager*  pCollisionManager_ = nullptr;

	std::vector<class SceneLayer*> sceneLayers_;
	std::unique_ptr<RenderTexture> finalRenderTex_;

private:
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};
