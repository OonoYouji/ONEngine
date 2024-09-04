#include "Scene_Game.h"

#include <ImGuiManager.h> 
#include <ModelManager.h>

#include "Player/Player.h"


class Tmp : public BaseGameObject {
public:
	Tmp() {}
	~Tmp() {}
	void Initialize() override {
		CreateTag(this);
		model_ = ModelManager::CreatePlane();
		CreateSphereCollider(model_);
	}
	void Update() override {}
	void Draw() override {
		model_->Draw(&transform_);
	}
private:
	Model* model_ = nullptr;
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
	
	transform_.Initialize();

	Player* player = new Player;
	player->Initialize();
	player->SetPositionX(-3.46f);



	(new Tmp)->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {



	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	//model_->Draw(&transform_);

	sprite_->Draw();

}
