#include "Scene_Title.h"

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"


void Scene_Title::Initialize() {


}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::GAME);
	}

}
