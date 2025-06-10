#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Scene/IScene.h"

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

	void LoadLevel(const std::string& _levelName);

	IEntity* LoadEntity(const nlohmann::json& _entityData);

};

