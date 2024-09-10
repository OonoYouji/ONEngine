#include "Scene_Game.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include <SceneManager.h>
#include <Audio/AudioManager.h>

#include "Player/Player.h"
#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Shake/Shake.h"
#include "Player/PlayerHP.h"
#include "GameCameraState/GameCameraState.h"
#include "GameResult/GameResult.h"
#include "LineDrawer2D/SinWaveDrawer.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyManager.h"


class Background : public BaseGameObject {
public:
	Background() { CreateTag(this); }
	~Background() {}
	void Initialize() override {
		sprite_.reset(new Sprite);
		sprite_->Initialize("white2x2", "white2x2s.png");
		transform_.position = Vec3(640, 360, 0);
		transform_.scale = Vec3(1280, 720, 1);
		sprite_->SetPos(GetPosition());
		sprite_->SetSize({ transform_.scale.x, transform_.scale.y });
		sprite_->SetColor({ 0,0,0,1 });
	}
	void Update() override {
		UpdateMatrix();
		sprite_->SetPos(GetPosition());
		sprite_->SetSize({ transform_.scale.x, transform_.scale.y });
		sprite_->SetColor(color);
		sprite_->SetAngle(angle_);
	}
	void BackSpriteDraw() override {
		sprite_->Draw();
	}
	void Debug() override {
		if(ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("angle", &angle_);
			ImGui::ColorEdit4("color", &color.x);
			ImGui::TreePop();
		}
	}
private:
	std::unique_ptr<Sprite> sprite_;
	float angle_ = 0.0f;
	Vec4 color = { 0,0,0,1 };
};

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	/// 波
	SinWaveDrawer* sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

	/// 敵
	Enemy* enemy_ = new Enemy();
	enemy_->SetWave(sinWave_);
	enemy_->Initialize();

	/// 敵pop
	EnemyManager* enemyManager_ = new EnemyManager();
	enemyManager_->Initialize();

	/// プレイヤーのHP
	PlayerHP* playerHP = new PlayerHP();
	playerHP->Initialize();

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


	Background* gameBG = new Background();
	gameBG->Initialize();
	gameBG->drawLayerId = 1;

	(new Background)->Initialize();
	(new GameCameraState)->Initialize();
	(new GameResult)->Initialize();



}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


