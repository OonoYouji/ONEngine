#pragma once

#include <vector>

#include <Graphics/SceneLayer/SceneLayer.h>

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

protected:
	class BaseCamera*       mainCamera_       = nullptr;
	class DirectionalLight* directionalLight_ = nullptr;

	std::vector<std::unique_ptr<SceneLayer>> layers_;
};