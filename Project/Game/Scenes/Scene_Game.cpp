#include "Scene_Game.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Player/Player.h"
#include "Objects/ShootingCourse/ShootingCourse.h"
#include "Objects/RailCamera/RailCamera.h"
#include "Objects/Reticle/Reticle.h"
#include "Objects/PlayerBullet/PlayerBullet.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// camera setting
	mainCamera_->SetPosition({});
	mainCamera_->SetRotate({});


	/// instance create...
	ShootingCourse* shootingCourse = new ShootingCourse();
	RailCamera*     railCamera     = new RailCamera();
	Reticle*        reticle        = new Reticle();
	Player*         player         = new Player();

	/// instance initializing...
	shootingCourse->Initialize();
	railCamera->Initialize();
	reticle->Initialize();
	player->Initialize();
	
	/// その他ポインタ設定など...

	railCamera->SetGameCamera(mainCamera_);
	railCamera->SetShootingCourse(shootingCourse);
	player->SetShootingCourse(shootingCourse);

	reticle->SetGameCamera(mainCamera_);


}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	

}
