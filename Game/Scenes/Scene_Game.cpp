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

#include<numbers>
/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	//プレイヤー
	 p1 = new Player;
	 buildingManager_ = new BuildingManager;
	
	Ground* ground = new Ground;
	p1->Initialize();
	ground->Initialize();
	mainCamera_->Initialize();
	//ペアレント
	mainCamera_->SetParent(p1->GetPivot());
	mainCamera_->SetPosition({ 0.0f, 0, -50.0f });
	mainCamera_->SetRotate({ 45, 0, 0.0f });
	mainCamera_->UpdateMatrix();

	//ビル生成
	buildingManager_->SpownBuilding(std::numbers::pi_v<float>/8.0f, std::numbers::pi_v<float> / 2.0f);
	buildingManager_->SpownBuilding(0.9f,6.5f);
}

/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	


	buildingManager_->Update();

	mainCamera_->UpdateMatrix();
}
