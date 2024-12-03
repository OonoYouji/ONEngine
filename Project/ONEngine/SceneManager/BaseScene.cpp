#include "BaseScene.h"

#include "Objects/Camera/GameCamera.h"

#include "GraphicManager/Light/DirectionalLight.h"
#include "Objects/Camera/Manager/CameraManager.h"

#include "SceneManager.h"


BaseScene::BaseScene() {

	/// layer  initialize
	layers_["default"] = (std::make_unique<SceneLayer>());
	layers_["default"]->Initialize("default", mainCamera_);

	CreateObject();
}

std::vector<SceneLayer*> BaseScene::GetSceneLayers() {
	std::vector<SceneLayer*> result{};
	for(auto& sceneLayer : layers_) {
		result.push_back(sceneLayer.second.get());
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

	layers_["default"]->SetMainCamera(mainCamera_);
}

void BaseScene::SetNextScene(const std::string& _sceneName) {
	SceneManager::GetInstance()->SetNextScene(_sceneName);
}



void BaseScene::AddLayer(const std::string& layerName, GameCamera* layerCamera) {
	auto itr = layers_.find(layerName);
	if(itr == layers_.end()) {
		layers_[layerName] = (std::make_unique<SceneLayer>());
		layers_[layerName]->Initialize(layerName, layerCamera);
	} else {
		SceneLayer* layer = itr->second.get();
		layer->SetMainCamera(layerCamera);
	}
}

void BaseScene::AddPostEffect(const std::string& _layerName, const std::string& _postEffectName) {
	BasePostEffectPipelineRegistry* pipelineRegistry = SceneManager::GetInstance()->GetPostEffectPipelineRegistry();
	BasePostEffectPipeline*         pipeline         = pipelineRegistry->GetPipeline(_postEffectName);

	layers_.at(_layerName)->AddPostEffectPipeline(pipeline);
}
