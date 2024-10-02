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
#include <CollisionManager/CollisionManager.h>
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
	pGameObjectManager_ = GameObjectManager::GetInstance();
	Load(currentId_);

	pCollisionManager_ = CollisionManager::GetInstance();
}


/// ===================================================
/// 終了処理
/// ===================================================
void SceneManager::Finalize() {
	//scenes_.clear();
	for(auto& scene : scenes_) {
		scene.reset();
	}
}


/// ===================================================
/// 更新処理
/// ===================================================
void SceneManager::Update() {
	if(currentId_ != nextSceneId_) {
		Load(nextSceneId_);
	}

	scenes_[currentId_]->Update();

	/// 更新1
	pGameObjectManager_->Update();
	/// 当たり判定処理
	pCollisionManager_->Update();
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

	const char* labels[4]{ "Title", "Game", "Result", "Clear"};
	int currentItem = static_cast<int>(nextSceneId_);
	if(ImGui::Combo("next scene", &currentItem, labels, 4)) {
		nextSceneId_ = SCENE_ID(currentItem);
		Load(nextSceneId_);
	}

	if(ImGui::TreeNodeEx("Layer")) {
		for(auto& layer : sceneLayers_) {
			layer->ImGuiDebug();
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void SceneManager::SetNextScene(SCENE_ID nextId) {
	nextSceneId_ = nextId;
}

void SceneManager::SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers) {
	sceneLayers_ = sceneLayers;
}

DirectionalLight* SceneManager::GetDirectionalLight() {
	return scenes_[currentId_]->directionalLight_;
}

void SceneManager::Load(SCENE_ID id) {

	currentId_ = id;
	auto SceneCreate = [&]() -> BaseScene* {
		switch(id) {
		case TITLE:
			return new Scene_Title;
		case GAME:
			return new Scene_Game;
		case RESULT:
			return new Scene_Result;
		case CLEAR:
			return new Scene_Clear;
		}
		return nullptr;
	};

	/// idに沿ったシーンの作成
	if(!scenes_[currentId_]) {
		scenes_[currentId_].reset(SceneCreate());
	} else {
		scenes_[currentId_]->CreateObject();
	}
	

	GameObjectManager::DestoryAll();
	CollisionManager::GetInstance()->Reset();

	scenes_[currentId_]->Initialize();

	ModelManager::GetInstance()->SetDirectionalLight(scenes_[currentId_]->directionalLight_);
	SetSceneLayers(scenes_[currentId_]->GetSceneLayers());

	GameObjectManager::AddObjectsToObjectsCopy();;
}


