#include "Scene_Game.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"
#include "SceneManager/SceneManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Math/Random.h"

/// game
#include "GraphicManager/Light/DirectionalLight.h"
#include "GraphicManager/Light/PointLight.h"
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"
#include "Objects/Player/Player.h"
#include "Objects/EnemyManager/EnemyManager.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/EnemyEmitter/EnemyEmitter.h"
#include "Objects/TrackingCamera/TrackingCamera.h"
#include "Objects/Player/PlayerOperationGUI/PlayerOperationGUI.h"

#include "Objects/BackgroundObject/Stage/Stage.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

#ifdef _DEBUG
	{
		auto mesh = mainCamera_->AddComponent<MeshRenderer>();
		mesh->SetModel("Sphere");
	}
#endif // _DEBUG


	/// insatnce create
	std::vector<BaseGameObject*> createObjects;
	createObjects.reserve(16);
	createObjects.push_back(new Player(mainCamera_));
	createObjects.push_back(new TrackingCamera(mainCamera_, createObjects[0]));
	createObjects.push_back(new Stage());
	createObjects.push_back(new EnemyManager(static_cast<Player*>(createObjects[0])));
	createObjects.push_back(new PointLight());
	createObjects.push_back(new PointLight());
	createObjects.push_back(new PointLight());
	createObjects.push_back(new PointLight());
	createObjects.push_back(new PointLight());
	createObjects.push_back(new PlayerOperationGUI());

	/// initailizing
	for(BaseGameObject* obj : createObjects) {
		obj->Initialize();
	}

	enemyManager_ = static_cast<EnemyManager*>(createObjects[3]);
	Player* player = static_cast<Player*>(createObjects[0]);
	player->SetEnemyManager(enemyManager_);

	directionalLight_->SetDirection({ 0.0f, -1.0f, 0.0f });

	/// Layerの設定
	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);
	uiCamera->SetDistance(10.0f);

	AddLayer("UILayer", uiCamera);
	AddLayer("TargetSpriteLayer", mainCamera_);


	/// Lightの設定
	LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();
	//lightGroup->SetDirectionalLightBufferData(0, directionalLight_->GetData());

	for(int i = 0; i < 5; ++i) {
		/// 4番以降から5個のオブジェクトがライト
		pointLightArray_[i] = static_cast<PointLight*>(createObjects[4 + i]);
		pointLightArray_[i]->SetColor(Vec4(Random::Vec3({ 0.25f, 0.25f, 0.25f }, Vec3::kOne), 1.0f));
		pointLightArray_[i]->SetIntencity(0.5f);
	}

	for(int i = 0; i < 5; ++i) {
		lightGroup->SetPointLightBufferData(i, pointLightArray_[i]->GetData());
	}

	pointLightArray_[0]->SetPosition({ 0.0f, 8.0f, 0.0f });
	pointLightArray_[0]->SetColor({ 0.25f, 0.25f, 0.72f, 1.0f });
	
	pointLightArray_[1]->SetPosition({ -20.0f, 8.0f, -20.0f });
	pointLightArray_[2]->SetPosition({ -20.0f, 8.0f, +20.0f });
	pointLightArray_[3]->SetPosition({ +20.0f, 8.0f, -20.0f });
	pointLightArray_[4]->SetPosition({ +20.0f, 8.0f, +20.0f });


}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();
	lightGroup->SetDirectionalLightBufferData(0, directionalLight_->GetData());
	for(int i = 0; i < 5; ++i) {
		lightGroup->SetPointLightBufferData(i, pointLightArray_[i]->GetData());
	}

	/// ゲームが終了したのでシーン遷移
	if(enemyManager_->GetDefeatEnemiesCount() > 20) {
		SetNextScene("ClearScene");
	}

}
