#include "SceneManager.h"

/// directX
#include <d3dx12.h>

#include "Core/ONEngine.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"
#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"

#include "WindowManager/WinApp.h"
#include "ImGuiManager/ImGuiManager.h"
#include "GameObjectManager/GameObjectManager.h"
#include "CollisionManager/CollisionManager.h"
#include "AudioManager/AudioManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/SceneLayer/SceneLayer.h"

#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

#include "BaseScene.h"
#include "../Scene_Game.h"
#include "../Scene_Title.h"
#include "../Scene_Result.h"
#include "../Scene_Clear.h"



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

	auto dxCommon = ONEngine::GetDxCommon();

	finalRenderTex_.reset(new RenderTexture);
	finalRenderTex_->Initialize(
		{0.0f, 0.0f, 0.0f, 0.0f},
		dxCommon->GetDxCommand()->GetList(), 
		dxCommon->GetSRVDescriptorHeap(),
		dxCommon->GetRTVDescriptorHeap(),
		dxCommon->GetDSVDescriptorHeap()
	);

}


/// ===================================================
/// 終了処理
/// ===================================================
void SceneManager::Finalize() {
	finalRenderTex_.reset();
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

	std::vector<RenderTexture*> renderTextures;
	for(auto& layer : sceneLayers_) {

		/// 描画
		layer->Draw();

		/// render textureを追加
		renderTextures.push_back(layer->GetRenderTexture());
	}


	/// 最終的なrender textureを作る
	RenderTextureManager::CreateBlendRenderTexture(
		renderTextures, finalRenderTex_.get()
	);
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

	/// 必要な変数のリセットをかける
	SceneLayer::ResetInstanceCount();

	currentId_ = id;
	auto SceneCreate = [&]() -> BaseScene* {
		switch(id) {
		case TITLE:
			return new Scene_Title();
		case GAME:
			return new Scene_Game();
		case RESULT:
			return new Scene_Result();
		case CLEAR:
			return new Scene_Clear();
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

	MeshInstancingRenderer::SetDirectionalLight(scenes_[currentId_]->directionalLight_);
	ModelManager::GetInstance()->SetDirectionalLight(scenes_[currentId_]->directionalLight_);
	SetSceneLayers(scenes_[currentId_]->GetSceneLayers());

	GameObjectManager::AddObjectsToObjectsCopy();;
}


