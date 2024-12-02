#pragma once

#include <vector>

#include <GraphicManager/SceneLayer/SceneLayer.h>
#include <string>

/// ===================================================
/// 各シーンの基底クラス
/// ===================================================
class BaseScene {
	friend class SceneManager;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseScene();
	virtual ~BaseScene() = default;

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// layers_のポインタを返す
	/// </summary>
	/// <returns></returns>
	std::vector<SceneLayer*> GetSceneLayers();

	void CreateObject();

protected:

	/// ===================================================
	/// protected : methods
	/// ===================================================
	
	/// <summary>
	/// 次のシーンを設定する
	/// </summary>
	/// <param name="_sceneName"></param>
	void SetNextScene(const std::string& _sceneName);

	/// <summary>
	/// 描画用レイヤーの追加
	/// </summary>
	/// <param name="layerName"></param>
	/// <param name="layerCamera"></param>
	void AddLayer(const std::string& layerName, class GameCamera* layerCamera);

	/// <summary>
	/// ポストエフェクトの追加
	/// </summary>
	/// <param name="_layerName">      : ポストエフェクトを追加するレイヤーの名前 </param>
	/// <param name="_postEffectName"> : 追加するポストエフェクトの名前         </param>
	void AddPostEffect(const std::string& _layerName, const std::string& _postEffectName);

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	class GameCamera*       mainCamera_       = nullptr;
	class DirectionalLight* directionalLight_ = nullptr;

	std::vector<std::unique_ptr<SceneLayer>> layers_;

	class SceneManager*     pSceneManager_ = nullptr;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================


};