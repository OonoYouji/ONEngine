#include "Scene_Title.h"

/// engine
#include "Core/ONEngine.h"
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/GameSelector/GameSelector.h"
#include "Objects/TitleSprite/TitleSprite.h"

void Scene_Title::Initialize() {

	gameSelector_ = new GameSelector();
	gameSelector_->Initialize();
	
	(new TitleSprite)->Initialize();

	mainCamera_->SetProjectionType(ORTHOGRAPHIC);
	mainCamera_->SetDistance(10.0f);
	mainCamera_->SetPositionY(0.5f);

}

void Scene_Title::Update() {

	/// ゲームシーンに遷移
	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager* sceneManager = SceneManager::GetInstance();

		switch(gameSelector_->GetGameSelectMode()) {
		case GAME_SELECT_MODE_START:

			sceneManager->SetNextScene(GAME);
			break;
		case GAME_SELECT_MODE_EXIT:

			ONEngine::SetIsRunning(false);
			break;
		}
	}

}
