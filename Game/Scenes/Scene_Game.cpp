#include "Scene_Game.h"

#include <ImGuiManager/ImGuiManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>
#include "Particle/ParticleSystem.h"

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"
//class
#include"GameCamera.h"
#include"Ground/Ground.h"
//#include"Matrix4x4.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	 p1 = new Player;
	
	Ground* ground = new Ground;
	p1->Initialize();
	ground->Initialize();
	mainCamera_->Initialize();
	//ペアレント
	mainCamera_->SetParent(p1->GetPivot());
	mainCamera_->SetPosition({ 0.0f, 0, -30 });
	mainCamera_->SetRotate({ 0, 0, 0.0f });
	mainCamera_->UpdateMatrix();
}

/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	/*mainCamera_->SetPosition({ p1->GetPivot()->position.x, p1->GetPivot()->position.y, -30 });
	mainCamera_->SetRotate(p1->GetPivot()->rotate);*/
	//mainCamera_->SetQuaternion(p1->GetPivot()->quaternion.Inverse() * mainCamera_->GetTransform()->quaternion);
	mainCamera_->UpdateMatrix();
}
