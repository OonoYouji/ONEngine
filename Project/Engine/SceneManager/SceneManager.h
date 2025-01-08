#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "IScene.h"


/// ===================================================
/// シーンの管理を行うクラス
/// ===================================================
class SceneManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneManager();
	~SceneManager();

	void Initialize();
	void Update();
	void Render();

	void SetNextScene(IScene* _nextScene);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<IScene> currentScene_ = nullptr;
	std::unique_ptr<IScene> nextScene_    = nullptr;

	/// factory

};

