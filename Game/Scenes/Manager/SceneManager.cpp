#include <SceneManager.h>

#include <d3dx12.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDevice.h>
#include <DxDescriptor.h>
#include <DxBarrierCreator.h>

#include <ImGuiManager.h>
#include <GameObjectManager.h>
#include <Collision/CollisionManager.h>

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
void SceneManager::Initialize(SCENE_ID sceneId) {
	currentId_ = sceneId;
	scene_.reset(new Scene_Game());
	scene_->Initialize();

	pGameObjectManager_ = GameObjectManager::GetInstance();
	pCollisionManager_ = CollisionManager::GetInstance();
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

	/// 更新1
	pGameObjectManager_->Update(currentId_);
	/// 当たり判定処理
	pCollisionManager_->Update(currentId_);
	/// 更新2
	pGameObjectManager_->LastUpdate(currentId_);
}


/// ===================================================
/// 描画処理
/// ===================================================
void SceneManager::Draw() {
	scene_->Draw();
}

void SceneManager::SetNextScene(SCENE_ID nextId) {
	currentId_ = nextId;
}


