#include "Scene_Game.h"

#include <ImGuiManager.h> 
#include <ModelManager.h>

#include "Player/Player.h"
#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"


class Tmp : BaseGameObject{
public:
	Tmp() { CreateTag(this); }
	~Tmp(){}
	void Initialize() override { 
		drawLayerId = 1;
		heart_[0] = ModelManager::Load("HeartAbove");
		heart_[1] = ModelManager::Load("HeartBottom");
	}
	void Update() override {
		//sprite_->SetPos(GetPosition());
	}
	void Draw() override { 
		//sprite_->Draw();
		heart_[0]->Draw(&transform_);
		heart_[1]->Draw(&transform_);
	}
private:
	//std::unique_ptr<Sprite> sprite_;
	Model* heart_[2];
};


Scene_Game::Scene_Game() {}
Scene_Game::~Scene_Game() {

	delete sinWave_;

}


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


	//(new Tmp)->Initialize();
	(new Heart)->Initialize();
	(new GameMonitor)->Initialize();

	sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
	sinWave_->Update();

	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);
	sprite_->Draw();

	//sinWave_->Draw();

}
