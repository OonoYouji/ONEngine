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

	BaseScene();
	virtual ~BaseScene() = default;

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

private:

	/// <summary>
	/// layers_のポインタを返す
	/// </summary>
	/// <returns></returns>
	std::vector<SceneLayer*> GetSceneLayers();

	void CreateObject();


protected:

	void SetNextScene(const std::string& _sceneName);

	void AddLayer(const std::string& layerName, class GameCamera* layerCamera);

protected:
	class GameCamera*       mainCamera_       = nullptr;
	class DirectionalLight* directionalLight_ = nullptr;

	std::vector<std::unique_ptr<SceneLayer>> layers_;
};