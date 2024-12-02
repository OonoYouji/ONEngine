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
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

#include "BaseScene.h"
#include "Scenes/Scene_Game.h"
#include "Scenes/Scene_Title.h"
#include "Scenes/Scene_Result.h"
#include "Scenes/Scene_Clear.h"



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
void SceneManager::Initialize(AbstructSceneFactory * _sceneFactory) {

	sceneFactory_.reset(_sceneFactory);

	currentSceneName_ = sceneFactory_->GetStartupSceneName();
	nextSceneName_ = currentSceneName_;
	pGameObjectManager_ = GameObjectManager::GetInstance();
	Load(currentSceneName_);

	pCollisionManager_ = CollisionManager::GetInstance();

	auto dxCommon = ONEngine::GetDxCommon();

	finalRenderTex_.reset(new RenderTexture);
	finalRenderTex_->Initialize(
		{0,0,0, 0.0f},
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
	scenes_.clear();
}


/// ===================================================
/// 更新処理
/// ===================================================
void SceneManager::Update() {
	if(currentSceneName_ != nextSceneName_) {
		Load(nextSceneName_);
	}

	scenes_[currentSceneName_]->Update();

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

	/*const char* labels[4]{ "Title", "Game", "Result", "Clear"};
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
	}*/

	ImGui::End();
}

void SceneManager::SetNextScene(const std::string& _nextSceneName) {
	nextSceneName_ = _nextSceneName;
}

void SceneManager::SetSceneLayers(const std::vector<class SceneLayer*>& sceneLayers) {
	sceneLayers_ = sceneLayers;
}

DirectionalLight* SceneManager::GetDirectionalLight() {
	return scenes_[currentSceneName_]->directionalLight_;
}

void SceneManager::Load(const std::string& _sceneName) {
	/// 必要な変数のリセットをかける
	SceneLayer::ResetInstanceCount();

	currentSceneName_ = _sceneName;

	/// idに沿ったシーンの作成
	if(!scenes_[currentSceneName_]) {
		scenes_[currentSceneName_].reset(sceneFactory_->CreateScene(_sceneName));
	} else {
		scenes_[currentSceneName_]->CreateObject();
	}


	GameObjectManager::DestoryAll();
	CollisionManager::GetInstance()->Reset();

	scenes_[currentSceneName_]->Initialize();

	AnimationRendererCommon::GetInstance()->SetDirectionalLight(scenes_[currentSceneName_]->directionalLight_);
	MeshInstancingRenderer::SetDirectionalLight(scenes_[currentSceneName_]->directionalLight_);
	ModelManager::GetInstance()->SetDirectionalLight(scenes_[currentSceneName_]->directionalLight_);
	SetSceneLayers(scenes_[currentSceneName_]->GetSceneLayers());

	GameObjectManager::AddObjectsToObjectsCopy();;
}

