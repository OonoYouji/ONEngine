#include "Scene_Result.h"


#include "Scenes/Manager/SceneManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/ResultSprite/ResultSprite.h"
#include "Objects/ResultScore/ResultScore.h"
#include "Objects/ResultToTitle/ResultToTitle.h"


void Scene_Result::Initialize() {

	(new ResultSprite)->Initialize();
	(new ResultScore)->Initialize();
	(new ResultToTitle)->Initialize();

	mainCamera_->SetProjectionType(ORTHOGRAPHIC);
	mainCamera_->SetDistance(10.0f);

	mainCamera_->SetPosition({ 0.0f, 0.3f, 0.0f });


}

void Scene_Result::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(TITLE);
	}

}
