#include "Scene_Game.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Player/Player.h"
#include "Objects/ShootingCourse/ShootingCourse.h"
#include "Objects/RailCamera/RailCamera.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// camera setting
	mainCamera_->SetPosition({});
	mainCamera_->SetRotate({});
	MeshRenderer* mr = mainCamera_->AddComponent<MeshRenderer>();
	mr->SetModel("axis");


	/// instance create...
	Player*         player         = new Player();
	ShootingCourse* shootingCourse = new ShootingCourse();
	RailCamera*     railCamera     = new RailCamera();


	/// instance initializing...
	player->Initialize();
	shootingCourse->Initialize();
	railCamera->Initialize();
	
	/// その他ポインタ設定など...
	player->SetShootingCourse(shootingCourse);

	railCamera->SetGameCamera(mainCamera_);
	railCamera->SetShootingCourse(shootingCourse);




}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
