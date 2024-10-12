#include "Scene_Title.h"

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/DemoObject/DemoObject.h"


void Scene_Title::Initialize() {

	/// intance create
	DemoObject* demoObject = new DemoObject();


	/// initializing
	demoObject->Initialize();


	/// setting...

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::GAME);
	}

}
