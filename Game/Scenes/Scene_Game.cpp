#include "Scene_Game.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>

#include "Player/Player.h"
#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Shake/Shake.h"

#include <Input.h>

class Background : BaseGameObject {
public:
	Background() { CreateTag(this); }
	~Background() {}
	void Initialize() override {
		sprite_.reset(new Sprite);
		sprite_->Initialize("uvChecker", "uvChecker.png");
		sprite_->SetPos({ 640, 360,0 });
		sprite_->SetSize({1280, 720});
	}
	void Update() override {}
	void FrontSpriteDraw() override {
		sprite_->Draw();
	}
private:
	std::unique_ptr<Sprite> sprite_;
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

	Player* player = new Player;
	player->Initialize();
	player->drawLayerId = 0;


	(new Background)->Initialize();
	(new Heart)->Initialize();
	(new Hand)->Initialize();
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;

	sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

	enemy_ = new Enemy();
	enemy_->SetWave(sinWave_);
	enemy_->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		BaseCamera* camera =
			CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->GetShake()->Start();
	}

	sinWave_->Update();


	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);
	sprite_->Draw();

}
