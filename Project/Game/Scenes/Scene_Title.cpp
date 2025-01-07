#include "Scene_Title.h"

/// engine
#include "Input/Input.h"

void Scene_Title::Initialize() {

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SetNextScene("GameScene");
	}

}
