#include "Scene_Title.h"

/// engine
#include "Input/Input.h"

/// user
#include "Objects/TitleSprite/TitleSprite.h"
#include "Objects/Camera/GameCamera.h"

void Scene_Title::Initialize() {

	TitleSprite* sprite = new TitleSprite();
	sprite->Initialize();
	sprite->drawLayerId = TITLE_SCENE_LAYER_UI;


	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);
	uiCamera->SetDistance(10.0f);

	AddLayer("uiLayer", uiCamera);

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)|| Input::TriggerPadButton(PadCode::A)) {
		SetNextScene("GameScene");
	}

}
