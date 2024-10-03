#include "Scene_Game.h"

#include <ImGuiManager/ImGuiManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>
#include "Particle/ParticleSystem.h"

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"
//class
#include"GameCamera.h"
#include "DemoObject/DemoObject.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	///
	(new DemoObject)->Initialize();
	
	mainCamera_->SetPosition({ -1.4f, 0.0f, -14.0f });
	mainCamera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	mainCamera_->UpdateMatrix();
	
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
