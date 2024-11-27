#include "Scene_Clear.h"

/// engine
#include "Scenes/Manager/SceneManager.h"
#include "Input/Input.h"

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
#include "Objects/BGMObj/BGMObj.h"


void Scene_Clear::Initialize() {
	std::list<BaseGameObject*> objects = {};

	objects.push_back(new ResultText("GameClearText.png"));
	objects.push_back(new ResultGameClearTimeRenderer(GameManagerObject::GetClearTime()));
	objects.push_back(new ResultToTitleText());
	objects.push_back(new ResultArrow());
	objects.push_back(new TitleSelectorUI());
	objects.push_back(new BGMObj("BGMs/Result.wav"));

	for(auto& object : objects) {
		object->Initialize();
		object->drawLayerId = CLEAR_LAYER_UI;
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


void Scene_Clear::Update() {


	/// ゲームを開始した
	//if(menuSelector_->GetIsSelected()) {
	bool isTransitio = false;
	isTransitio |= Input::TriggerKey(KeyCode::Space);
	isTransitio |= Input::TriggerPadButton(PadCode::A);

	if(isTransitio) {
		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, CLEAR_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		}
	}



	if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
		SceneManager* sceneManager = SceneManager::GetInstance();
		sceneManager->SetNextScene(TITLE);
	}
}
