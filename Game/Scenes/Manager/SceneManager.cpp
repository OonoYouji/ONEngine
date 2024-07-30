#include <SceneManager.h>


#include <BaseScene.h>
#include <Scene_Game.h>


/// ===================================================
/// インスタンス確保
/// ===================================================
SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}


/// ===================================================
/// 初期化処理
/// ===================================================
void SceneManager::Initialize() {
	scene_.reset(new Scene_Game());
	scene_->Initialize();
	
}


/// ===================================================
/// 終了処理
/// ===================================================
void SceneManager::Finalize() {
	scene_.reset();
}


/// ===================================================
/// 更新処理
/// ===================================================
void SceneManager::Update() {
	scene_->Update();
}


/// ===================================================
/// 描画処理
/// ===================================================
void SceneManager::Draw() {
	scene_->Draw();
}
