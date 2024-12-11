#include "Scene_Game.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// game
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"
#include "Objects/Player/Player.h"
#include "Objects/TrackingCamera/TrackingCamera.h"

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

	(new DemoObject)->Initialize();

	/// insatnce create
	std::vector<BaseGameObject*> createObjects;
	createObjects.reserve(16);
	createObjects.push_back(new Player(mainCamera_));
	createObjects.push_back(new TrackingCamera(mainCamera_, createObjects[0]));
	createObjects.push_back(new Stage());

	/// initailizing
	for(BaseGameObject* obj : createObjects) {
		obj->Initialize();
	}
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	/*/// ゲームが終了したのでシーン遷移
	if(gameManager_->GetIsGameEnd()) {
		
		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, SCENE_GAME_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		} else {

			if(sceneTransition_->GetIsEnd()) {

				SceneManager::GetInstance()->SetNextScene(RESULT);
			}

		}

	} else {


		/// ゲーム開始の処理
		if(!isStarted_) {
			isStarted_ = true;

			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_OUT, 2.0f, SCENE_GAME_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		} else {

			if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
				railCamera_->isActive = true;

				sceneTransition_->Destory();
				sceneTransition_ = nullptr;
			}
		}
	}*/


}
