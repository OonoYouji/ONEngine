#include "Scene_Game.h"

#include "Objects/Player/Player.h"
#include "Objects/ShootingCourse/ShootingCourse.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// instance create...
	Player*         player         = new Player();
	ShootingCourse* shootingCourse = new ShootingCourse();


	/// instance initializing...
	player->Initialize();
	shootingCourse->Initialize();
	
	/// その他ポインタ設定など...
	player->SetShootingCourse(shootingCourse);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
