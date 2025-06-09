#pragma once

/// engine
#include "Engine/Scene/IScene.h"

/// ///////////////////////////////////////////////////
/// シーンのロードレベルテスト
/// ///////////////////////////////////////////////////
class LoadLevelTestScene : public IScene {
public:

	LoadLevelTestScene() = default;
	~LoadLevelTestScene() = default;

	void Initialize() override;
	void Update() override;

private:

	void LoadLevel(const std::string& _levelName);

};

