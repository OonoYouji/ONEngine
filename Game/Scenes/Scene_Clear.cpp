#include "Scene_Clear.h"


#include "Background/Background.h"
#include "GameMonitor/GameMonitor.h"
#include "Hand/Hand.h"
#include "Heart/Heart.h"
#include "Tile/Tile.h"
#include "Wall/Wall.h"



void Scene_Clear::Initialize() {

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


	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;

	(new Background)->Initialize();
	(new Tile)->Initialize();
	(new Wall)->Initialize();

}



void Scene_Clear::Update() {

}
