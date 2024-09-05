#include <Scene_Debug.h>

#include <CameraManager.h>
#include <SceneManager.h>
#include <ModelManager.h>
#include <SpriteManager.h>

#include <DebugCamera.h>
#include <Floor.h>


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

void Scene_Debug::Draw() {
	/*cameraManager_->SetMainCamera("DebugCamera");
	sceneManager_->SetRenderTarget(kDebugScene);
	modelManager_->PostDraw();
	spriteManager_->PostDraw();
	floor_->Draw();
	cameraManager_->SetMainCamera("GameCamera");*/
}
