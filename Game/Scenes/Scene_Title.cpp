#include "Scene_Title.h"

#include <CameraManager.h>
#include <Input.h>

#include "GameMonitor/GameMonitor.h"
#include "Background/Background.h"
#include "GameTitle/GameTitle.h"

void Scene_Title::Initialize() {

	/// カメラの座標を設定
	BaseCamera* camera =
		CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
		{ {0.0f, 0.3f, -7.6f}, { 0.0f, 0.0f, 0.0f } },
		0.0f
	);


	/// ===================================================
	/// タイトルシーンのオブジェクトを初期化
	/// ===================================================

	(new GameTitle)->Initialize();

	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();

	Background* gameBG = new Background;
	gameBG->Initialize();
	gameBG->drawLayerId = 1;

	Background* monitorBG = new Background;
	monitorBG->Initialize();
	monitorBG->SetColor({ 0, 0, 0, 1 });

	//(new Wave)->Initialize();

}

void Scene_Title::Update() {}
