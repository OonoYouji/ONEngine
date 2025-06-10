#pragma once

/// engine
#include "Engine/Scene/IScene.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"

/// ///////////////////////////////////////////////////
/// シーンのロードレベルテスト
/// ///////////////////////////////////////////////////
class LoadLevelTestScene : public IScene {
public:

	LoadLevelTestScene();
	~LoadLevelTestScene() = default;

	void Initialize() override;
	void Update() override;

private:
	std::unique_ptr<LoadSceneCommand> loadSceneCommand_;
};

