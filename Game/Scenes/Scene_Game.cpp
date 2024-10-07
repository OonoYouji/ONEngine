#include "Scene_Game.h"

#include <ImGuiManager/ImGuiManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>
#include "Particle/ParticleSystem.h"

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"
//class
#include "GameCamera.h"
#include "Ground/Ground.h"
#include "GameCameraState/GameCameraState.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_                           = new Player;
	Ground* ground                    = new Ground;
	GameCameraState* gameCameraState_ = new GameCameraState();



	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	ground->Initialize();
	gameCameraState_->Initialize();

	mainCamera_->SetPosition({ 0.0f, 0.0f, -50.0f });

	
	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	mainCamera_->SetParent(player_->GetPivot());

	gameCameraState_->SetGameCamera(mainCamera_);
	gameCameraState_->SetPlayer(player_);


}

/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {


}
