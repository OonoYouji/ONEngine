#include "Scene_Game.h"


#include <ImGuiManager/ImGuiManager.h>

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"
#include"Ground/Ground.h"

		Vector3 normalizedAxis = axis.Normalize();

		float w = cos(halfAngle);
		float x = normalizedAxis.x * sinHalfAngle;
		float y = normalizedAxis.y * sinHalfAngle;
		float z = normalizedAxis.z * sinHalfAngle;

		return Quaternion(x, y, z, w);
	}
};


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player* p1 = new Player;
	Ground* ground = new Ground;
	p1->Initialize();
	ground->Initialize();
	

	p1->SetParent(ground);

	mainCamera_->SetPosition({ 0.0f, 0.0f, -6.49f * 4 });
	mainCamera_->SetRotate({ 0.066f, -0.258f, 0.0f });
	mainCamera_->UpdateMatrix();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


