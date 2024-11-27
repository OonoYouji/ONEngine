#include "Scene_Result.h"

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/SceneTransition/SceneTransition.h"
#include "Objects/GameManagerObject/GameManagerObject.h"

/// game over objects
#include "Objects/ResultObjects/ResultText/ResultText.h"
#include "Objects/GameOverObjects/GameOverMenuSelector/GameOverMenuSelector.h"


void Scene_Result::Initialize() {

	std::vector<BaseGameObject*> objects = {};

	/// 結果で初期化するオブジェクトを変更
	objects.push_back(new ResultText("GameOverText.png"));
	objects.push_back(new GameOverMenuSelector());

	for(auto& object : objects) {
		object->Initialize();
		object->drawLayerId = RESULT_LAYER_UI;
	}

	selector_ = static_cast<GameOverMenuSelector*>(objects[1]);

	/// add layer ---------------------------------------------------------------------

	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);

	AddLayer("UILayer", uiCamera);



	GameCamera* transitionCamera = new GameCamera("TransitionCamera");
	transitionCamera->Initialize();
	transitionCamera->SetProjectionType(ORTHOGRAPHIC);
	AddLayer("TransitionLayer", transitionCamera);

	GameManagerObject::SetFlag("isGameRestart", false);

	sceneTransition_ = nullptr;
}

void Scene_Result::Update() {


	/// ゲームを開始した
	//if(menuSelector_->GetIsSelected()) {
	bool isTransitio = false;
	isTransitio |= Input::TriggerKey(KeyCode::Space);
	isTransitio |= Input::TriggerPadButton(PadCode::A);

	if(isTransitio) {

		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, RESULT_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		}
	}



	if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
		SceneManager* sceneManager = SceneManager::GetInstance();

		switch(selector_->GetSelectMenu()) {
		case SELECT_MENU_RESTART:
			sceneManager->SetNextScene(SCENE_ID::GAME);
			GameManagerObject::SetFlag("isGameRestart", true);
			break;
		case SELECT_MENU_TITLE:
			sceneManager->SetNextScene(SCENE_ID::TITLE);
			break;
		}
	}

}
