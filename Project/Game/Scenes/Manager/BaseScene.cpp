#include "Scenes/Manager/BaseScene.h"

#include "Objects/Camera/GameCamera.h"

#include "GraphicManager/Light/DirectionalLight.h"
#include "Objects/Camera/Manager/CameraManager.h"

BaseScene::BaseScene() {

	CreateObject();

	/// layer  initialize
	layers_.push_back(std::make_unique<SceneLayer>());
	layers_[0]->Initialize("default", mainCamera_);

}

std::vector<SceneLayer*> BaseScene::GetSceneLayers() {
	std::vector<SceneLayer*> result{};
	for(auto& sceneLayer : layers_) {
		result.push_back(sceneLayer.get());
	}
	return result;
}

void BaseScene::CreateObject() {
	/// camera initialize
	mainCamera_ = new GameCamera("MainCamera");
	mainCamera_->Initialize();

	//CameraManager::GetInstance()->SetMainCamera(mainCamera_);

	/// light  initialize
	directionalLight_ = new DirectionalLight();
	directionalLight_->Initialize();
}



void BaseScene::AddLayer(const std::string& layerName, GameCamera* layerCamera) {
	layers_.push_back(std::make_unique<SceneLayer>());
	layers_.back()->Initialize(layerName, layerCamera);
}
