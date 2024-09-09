#include "Scene_Game.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include <SceneManager.h>

#include "Player/Player.h"
#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Shake/Shake.h"
#include "Player/PlayerHP.h"
#include "GameCameraState/GameCameraState.h"
#include "GameResult/GameResult.h"

#include <Input.h>

class Background : BaseGameObject {
public:
	Background() { CreateTag(this); }
	~Background() {}
	void Initialize() override {
		sprite_.reset(new Sprite);
		sprite_->Initialize("uvChecker", "uvChecker.png");
		sprite_->SetPos({ 640, 360,0 });
		sprite_->SetSize({ 1280, 720 });
		sprite_->SetColor({ 0,0,0,1 });
	}
	void Update() override {
		sprite_->SetColor(color);
		sprite_->SetAngle(angle_);
	}
	void FrontSpriteDraw() override {
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

Scene_Game::Scene_Game() {}
Scene_Game::~Scene_Game() {}


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	model_ = ModelManager::CreateCube();
	model_->Initialize();

	sprite_.reset(new Sprite());
	sprite_->Initialize("uvChecker", "uvChecker.png");
	sprite_->SetColor({ 1,1,1,0.5f });

	transform_.Initialize();

	/// 波
	sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

	/// 敵
	enemy_ = new Enemy();
	enemy_->SetWave(sinWave_);
	enemy_->Initialize();

	/// 敵pop
	enemyManager_ = new EnemyManager();

	/// プレイヤーのHP
	PlayerHP* playerHP = new PlayerHP();
	playerHP->Initialize();

	/// 心臓
	Heart* heart = new Heart;
	heart->Initialize();
	heart->SetPosition({ -3.1f, 0.05f, -4.0f });
	heart->SetRotate({ 0.0f, -1.0f, 0.45f });
	heart->SetScale(Vec3::kOne * 0.7f);
	heart->UpdateMatrix();

	/// 手
	Hand* hand = new Hand;
	hand->Initialize();
	hand->SetPosition({ -3.4f, 0.1f, -3.9f });
	hand->SetRotate({ 0.0f, -0.5f, 0.0f });
	hand->SetScale(Vec3::kOne * 0.5f);
	hand->UpdateMatrix();


	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;


	(new Background)->Initialize();
	(new GameCameraState)->Initialize();
	(new GameResult)->Initialize();

	
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
	


	/*sinWave_->Update();*/


	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);
	sprite_->Draw();

}
