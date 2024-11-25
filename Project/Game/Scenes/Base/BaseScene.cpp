#include "BaseScene.h"

#include "Objects/Camera/GameCamera.h"

#include "GraphicManager/Light/DirectionalLight.h"
#include "Objects/Camera/Manager/CameraManager.h"

BaseScene::BaseScene() {

	/// layer  initialize
	layers_.push_back(std::make_unique<SceneLayer>());
	layers_[0]->Initialize("default", mainCamera_);

	CreateObject();
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

	/// light  initialize
	directionalLight_ = new DirectionalLight();
	directionalLight_->Initialize();

	layers_[0]->SetMainCamera(mainCamera_);
}



void BaseScene::AddLayer(const std::string& layerName, GameCamera* layerCamera) {
	auto itr = std::find_if(layers_.begin(), layers_.end(), [&](const std::unique_ptr<SceneLayer>& layer) {
		if(layerName == layer->GetName()) {
			return true;
		}
		return false;
	});

	if(itr == layers_.end()) {
		layers_.push_back(std::make_unique<SceneLayer>());
		layers_.back()->Initialize(layerName, layerCamera);
	} else {
		SceneLayer* layer = itr->get();
		layer->SetMainCamera(layerCamera);
	}
}
