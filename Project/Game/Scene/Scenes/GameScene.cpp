#include "GameScene.h"

#include <memory>
#include "Engine/Graphics/Shader/GraphicsPipeline.h"


GameScene::GameScene() {}
GameScene::~GameScene() {}


void GameScene::Initialize() {
	 
	std::unique_ptr<GraphicsPipeline> pipeline = std::make_unique<GraphicsPipeline>();

	pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	pipeline->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline->CreatePipeline(nullptr);
}

void GameScene::Update() {

}

void GameScene::Draw() {

}
