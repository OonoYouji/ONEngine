#pragma once

#include "Base/BaseScene.h"

#include <memory>

class CameraManager;
class SpriteManager;
class ModelManager;
class SceneManager;

class DebugCamera;
class Floor;

/// ===================================================
/// デバッグ用シーン
/// ===================================================
class Scene_Debug : public BaseScene {
public:

	Scene_Debug();
	~Scene_Debug();

	void Initialize() override;
	void Update() override;

private:

	SpriteManager* spriteManager_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	ModelManager* modelManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<Floor> floor_ = nullptr;

};