#include <BaseScene.h>

#include <GameCamera.h>
#include <Graphics/Light/DirectionalLight.h>

BaseScene::BaseScene() {

	/// camera initialize
	mainCamera_ = new GameCamera("MainCamera");
	mainCamera_->Initialize();

	/// light  initialize
	directionalLight_ = new DirectionalLight();
	directionalLight_->Initialize();

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
