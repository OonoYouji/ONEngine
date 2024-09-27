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
//#include <Collision/CollisionManager.h>
#include <AudioManager.h>
#include <ModelManager.h>

#include <BaseScene.h>
#include <Scene_Game.h>
#include <Scene_Title.h>
#include <Scene_Result.h>
#include <Scene_Clear.h>

#include <SceneLayer/SceneLayer.h>


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
	nextSceneId_ = currentId_;
	Load(currentId_);

	pGameObjectManager_ = GameObjectManager::GetInstance();
	//pCollisionManager_ = CollisionManager::GetInstance();
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
	if(currentId_ != nextSceneId_) {
		Load(nextSceneId_);
		currentId_ = nextSceneId_;
	}

	scene_->Update();

	/// 更新1
	pGameObjectManager_->Update();
	/// 当たり判定処理
	//pCollisionManager_->Update();
	/// 更新2
	pGameObjectManager_->LastUpdate();



}


void SceneManager::Draw() {
	for(auto& layer : sceneLayers_) {
		layer->Draw();
	}
}

void SceneManager::ImGuiDebug() {
	if(!ImGui::Begin("scene manager")) {
		ImGui::End();
		return;
	}

	const char* labels[3]{ "Title", "Game", "Result" };
	int currentItem = static_cast<int>(nextSceneId_);
	if(ImGui::Combo("next scene", &currentItem, labels, 3)) {
		nextSceneId_ = SCENE_ID(currentItem);
		Load(nextSceneId_);
	}

	ImGui::End();


	for(auto& layer : sceneLayers_) {
		layer->ImGuiDebug();
	}

}

void SceneManager::SetNextScene(SCENE_ID nextId) {
	nextSceneId_ = nextId;
}

void SceneManager::SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers) {
	sceneLayers_ = sceneLayers;
}

void SceneManager::Load(SCENE_ID id) {
	switch(id) {
	case TITLE:
		scene_.reset(new Scene_Title());
		break;
	case GAME:
		scene_.reset(new Scene_Game());
		break;
	case RESULT:
		scene_.reset(new Scene_Result());
		break;
	case CLEAR:
		scene_.reset(new Scene_Clear());
		break;

	}

	GameObjectManager::DestoryAll();

	scene_->Initialize();
	ModelManager::GetInstance()->SetDirectionalLight(scene_->directionalLight_);
	SetSceneLayers(scene_->GetSceneLayers());

	GameObjectManager::AddObjectsToObjectsCopy();;
}


