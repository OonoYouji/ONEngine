#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>
#include <vector>
#include "../../../ONEngine/Graphics/SceneLayer/SceneLayer.h"

class BaseScene;

using namespace Microsoft::WRL;


enum SCENE_ID {
	TITLE,	/// タイトルシーン
	GAME,	/// ゲームシーン
	RESULT,	/// リザルトシーン
	CLEAR,  /// ゲームクリア
	SCENE_ID_COUNT
};


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

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(SCENE_ID sceneId);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	

	void ImGuiDebug();


	void SetNextScene(SCENE_ID nextId);

	SCENE_ID GetCurrentScene() const { return currentId_; }


	bool GetIsRunning() const { return isRunning_; }
	void SetIsRunning(bool isRunning) { isRunning_ = isRunning; }

	void SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers);

	SceneLayer* GetSceneLayer(uint32_t layerId) const {
		return sceneLayers_.at(layerId);
	}

private:

	void Load(SCENE_ID id);

private:

	std::unique_ptr<BaseScene> scene_ = nullptr;
	SCENE_ID nextSceneId_;
	SCENE_ID currentId_;

	class GameObjectManager* pGameObjectManager_ = nullptr;
	class CollisionManager* pCollisionManager_ = nullptr;

	std::vector<class SceneLayer*> sceneLayers_;

	bool isRunning_ = true;

private:
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};
