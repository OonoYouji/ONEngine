#include "Scene_Clear.h"

/// engine
#include "Input/Input.h"

/// user
#include "Objects/Camera/GameCamera.h"
#include "Objects/ClearSprite/ClearSprite.h"


void Scene_Clear::Initialize() {

	ClearSprite* sprite = new ClearSprite();
	sprite->Initialize();
	sprite->drawLayerId = CLEAR_SCENE_LAYER_UI;

	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);
	uiCamera->SetDistance(10.0f);

	AddLayer("UILayer", uiCamera);

}


void Scene_Clear::Update() {

	if(Input::TriggerKey(KeyCode::Space) || Input::TriggerPadButton(PadCode::A)) {
		SetNextScene("TitleScene");
	}

}
