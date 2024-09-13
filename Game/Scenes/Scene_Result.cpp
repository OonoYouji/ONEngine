#include "Scene_Result.h"

#include <CameraManager.h>
#include <SceneManager.h>
#include <Input.h>

#include "GameMonitor/GameMonitor.h"
#include "GameResult/GameResult.h"
#include "Background/Background.h"
#include "Hand/Hand.h"
#include "Heart/Heart.h"
#include "GameResult/GameResult.h"


Scene_Result::~Scene_Result() {
	bgm_->StopAudioAll();
}

void Scene_Result::Initialize() {

	/// カメラの座標を計算
	BaseCamera* camera =
		CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ camera->GetPosition(), camera->GetRotate() },
		{ { 0.0f, 0.2f, -15.0f }, { 0.0f, -0.12f, 0.0f } },
		0.0f
	);

	/// 仮に変更するので一旦値だけコメントアウト
	//{ { 0.0f, 0.2f, -15.0f }, { 0.0f, -0.12f, 0.0f } },

	(new GameMonitor)->Initialize();
	(new Background)->Initialize();
	(new GameResult)->Initialize();


	bgm_ = new AudioSource;
	bgm_->SetAudioClip("GameOver.wav");
	bgm_->isLoop = true;
	bgm_->PlayAudio();
}

void Scene_Result::Update() {


}
