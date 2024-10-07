#include "Scene_Debug.h"

#include "Objects/Camera/Manager/CameraManager.h"
#include "Manager/SceneManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/Drawer/Sprite/SpriteManager.h"

#include "Objects/Camera/DebugCamera.h"
#include "GraphicManager/Drawer/Floor/Floor.h"


Scene_Debug::Scene_Debug() {}
Scene_Debug::~Scene_Debug() {
	floor_.reset();
}


void Scene_Debug::Initialize() {
	cameraManager_ = CameraManager::GetInstance();
	sceneManager_ = SceneManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	spriteManager_ = SpriteManager::GetInstance();

	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize();

	floor_.reset(new Floor());
	floor_->Initialize();

}

void Scene_Debug::Update() {

}

