#pragma once

/// engine
#include "Engine/Scene/IScene.h"

/// game
#include "Game/Objects/Terrain/Editor/TerrainEditor.h"

/// ===================================================
/// ゲームのメインシーン
/// ===================================================
class GameScene final : public IScene {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameScene(class EntityComponentSystem* _entityComponentSystem, const std::string& _name);
	~GameScene();

	void Initialize() override;
	void Update()     override;

private:
	std::unique_ptr<TerrainEditor> terrainEditor_ = nullptr; ///< TerrainEditorへのポインタ
};

