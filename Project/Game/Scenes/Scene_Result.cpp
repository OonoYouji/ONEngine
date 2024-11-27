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
#include "Objects/ResultObjects/ResultToTitleText/ResultToTitleText.h"
#include "Objects/ResultObjects/ResultArrow/ResultArrow.h"
#include "Objects/ResultObjects/ResultGameClearTimeRenderer/ResultGameClearTimeRenderer.h"

#include "Objects/TitleObjects/TitleSelectorUI/TitleSelectorUI.h"


void Scene_Result::Initialize() {

	std::list<BaseGameObject*> objects = {};

	/// 結果で初期化するオブジェクトを変更
	if(GameManagerObject::GetFlag("isGameClear").Press()) {

		objects.push_back(new ResultText("GameClearText.png"));
		objects.push_back(new ResultGameClearTimeRenderer(GameManagerObject::GetClearTime()));

	} else {

		objects.push_back(new ResultText("GameOverText.png"));
	}

	objects.push_back(new ResultToTitleText());
	objects.push_back(new ResultArrow());
	objects.push_back(new TitleSelectorUI());

	for(auto& object : objects) {
		object->Initialize();
		object->drawLayerId = RESULT_LAYER_UI;
	}


	/// add layer ---------------------------------------------------------------------

	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);

	AddLayer("UILayer", uiCamera);



	GameCamera* transitionCamera = new GameCamera("TransitionCamera");
	transitionCamera->Initialize();
	transitionCamera->SetProjectionType(ORTHOGRAPHIC);
	AddLayer("TransitionLayer", transitionCamera);


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
		sceneManager->SetNextScene(TITLE);

		//switch(menuSelector_->GetSelectMenu()) {
		//case SELECT_MENU_START:
		//	sceneManager->SetNextScene(GAME);
		//	break;
		//case SELECT_MENU_EXIT:
		//	ONEngine::SetIsRunning(false);
		//	break;
		//}
	}

}
