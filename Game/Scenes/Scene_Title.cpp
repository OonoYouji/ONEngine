#include "Scene_Title.h"

#include <CameraManager.h>
#include <Input.h>

#include "GameMonitor/GameMonitor.h"
#include "Background/Background.h"
#include "GameTitle/GameTitle.h"


void Scene_Title::Initialize() {

	/// ===================================================
	/// タイトルシーンのオブジェクトを初期化
	/// ===================================================

	(new GameTitle)->Initialize();

	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;

	Background* gameBG = new Background;
	gameBG->Initialize();
	gameBG->drawLayerId = 1;

	Background* monitorBG = new Background;
	monitorBG->Initialize();
	monitorBG->SetColor({ 0, 0, 0, 1 });


	

}

void Scene_Title::Update() {}
