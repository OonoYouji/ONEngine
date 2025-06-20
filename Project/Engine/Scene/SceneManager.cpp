#include "SceneManager.h"

/// std
#include <numbers>

/// engine
#include "Scene/Factory/SceneFactory.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"


SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pEntityComponentSystem_(entityComponentSystem_) {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {
	Camera* mainCamera = pEntityComponentSystem_->GenerateCamera();
	mainCamera->SetPosition(Vector3(477.0f, 474.0f, -400.0f));
	mainCamera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	mainCamera->SetCameraType(static_cast<int>(CameraType::Type3D));
	pEntityComponentSystem_->SetMainCamera(mainCamera);

	Camera* mainCamera2D = pEntityComponentSystem_->GenerateCamera();
	mainCamera2D->SetCameraType(static_cast<int>(CameraType::Type2D));
	pEntityComponentSystem_->SetMainCamera2D(mainCamera2D);

	pGraphicsResourceCollection_ = _graphicsResourceCollection;

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneFactory_->Initialize();


	SetNextScene(sceneFactory_->GetStartupSceneName());
	MoveNextToCurrentScene();

	sceneIO_ = std::make_unique<SceneIO>(pEntityComponentSystem_);

}

void SceneManager::Update() {

	/// 次のシーンが設定されていたら、シーンを切り替える
	if(nextScene_ != nullptr) {
		MoveNextToCurrentScene();
	}

	/// 現在のシーンの更新処理
	currentScene_->Update();

	//if (Input::TriggerKey(DIK_U)) {
	//	sceneIO_->Output(currentScene_.get());
	//} else if (Input::TriggerKey(DIK_I)) {
	//	pEntityComponentSystem_->RemoveEntityAll();
	//	sceneIO_->Input(currentScene_.get());
	//}

}

void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = sceneFactory_->CreateScene(_sceneName);
}

void SceneManager::MoveNextToCurrentScene() {
	currentScene_ = std::move(nextScene_);
	
	/// resourceの読み込み、解放をここで行う
	pGraphicsResourceCollection_->UnloadResources(currentScene_->unloadResourcePaths_);
	pGraphicsResourceCollection_->LoadResources(currentScene_->loadResourcePaths_);

	/// sceneに必要な情報を渡して初期化
	currentScene_->SetEntityComponentSystem(pEntityComponentSystem_);
	currentScene_->SetSceneManagerPtr(this);
	currentScene_->Initialize();

	nextScene_ = nullptr;
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}


