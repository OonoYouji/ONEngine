#include "Scene_Clear.h"


#include "Background/Background.h"
#include "GameMonitor/GameMonitor.h"
#include "Hand/Hand.h"
#include "Heart/Heart.h"
#include "Tile/Tile.h"
#include "Wall/Wall.h"

#include "GameClearEffect/GameClearEffect.h"



void Scene_Clear::Initialize() {

	/// 心臓
	Heart* heart = new Heart;
	heart->Initialize();
	heart->SetPosition({ -3.8f, -0.8f, -4.1f });
	heart->SetRotate({ 0.0f, -1.0f, 0.45f });
	heart->SetScale(Vec3::kOne * 0.7f);
	heart->UpdateMatrix();



	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;

	(new Background)->Initialize();
	(new Tile)->Initialize();
	(new Wall)->Initialize();
	(new GameClearEffect)->Initialize();

}



void Scene_Clear::Update() {

}
