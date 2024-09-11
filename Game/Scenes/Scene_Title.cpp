#include "Scene_Title.h"

#include <CameraManager.h>
#include <SceneManager.h>

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



	/// ブルームエフェクトを適用する
	auto monitorLayer = SceneManager::GetInstance()->GetSceneLayer(0);
	monitorLayer->SetIsApplyBloom(true, FRONT_SPRITE);
	monitorLayer->SetIntensity(2.0f, FRONT_SPRITE);
	monitorLayer->SetTexSize({ 0.002f, 0.002f }, FRONT_SPRITE);
	monitorLayer->SetRadius(16, FRONT_SPRITE);
	
	auto gameLayer = SceneManager::GetInstance()->GetSceneLayer(1);
	gameLayer->SetIsApplyBloom(true, OBJECT3D);
	gameLayer->SetIntensity(2.0f, OBJECT3D);
	gameLayer->SetTexSize({0.002f, 0.002f }, OBJECT3D);
	gameLayer->SetRadius(16, OBJECT3D);

}

void Scene_Title::Update() {}
