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


void Scene_Result::Initialize() {

	/// カメラの座標を計算
	/*BaseCamera* camera =
		CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ camera->GetPosition(), camera->GetRotate() },
		{ {0.0f, 0.0f, -7.0f}, { 0.0f, 0.0f, 0.0f } },
		0.5f
	);*/

	/// 仮に変更するので一旦値だけコメントアウト
	//{ { 0.0f, 0.2f, -15.0f }, { 0.0f, -0.12f, 0.0f } },

	(new GameMonitor)->Initialize();
	(new Background)->Initialize();
	(new GameResult)->Initialize();
	
	/// 心臓
	Heart* heart = new Heart;
	heart->Initialize();
	heart->SetPosition({ -3.8f, -0.8f, -4.1f });
	heart->SetRotate({ 0.0f, -1.0f, 0.45f });
	heart->SetScale(Vec3::kOne * 0.7f);
	heart->UpdateMatrix();

	/// 手
	Hand* hand = new Hand;
	hand->Initialize();
	hand->SetPosition({ -4.05f, -0.95f, -3.9f });
	hand->SetRotate({ 0.0f, -0.5f, 0.0f });
	hand->SetScale(Vec3::kOne * 0.5f);
	hand->UpdateMatrix();

	(new GameResult)->Initialize();

}

void Scene_Result::Update() {
	/// 仮
	if(Input::TriggerKey(KeyCode::Enter)) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::TITLE);
	}
}
