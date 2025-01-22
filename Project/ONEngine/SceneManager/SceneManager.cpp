#define NOMINMAX
#include "SceneManager.h"

/// directX
#include <d3dx12.h>

#include "Core/ONEngine.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"

#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/SceneLayer/SceneLayer.h"

#include "WindowManager/ConsoleManager.h"
#include "WindowManager/WinApp.h"
#include "ImGuiManager/ImGuiManager.h"
#include "GameObjectManager/GameObjectManager.h"
#include "CollisionManager/CollisionManager.h"
#include "AudioManager/AudioManager.h"

#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"



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


	lightGroup_.reset(new LightGroup());
	lightGroup_->Initialize();

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


	//ConsoleManager::GetInstance()->RegisterFunction([&](ImGuiWindowFlags _windowFlags) { Scene(_windowFlags); });
	ConsoleManager::GetInstance()->RegisterFunction([&](ImGuiWindowFlags _windowFlags) { DebugScene(_windowFlags); });
}


/// ===================================================
/// 終了処理
/// ===================================================
void SceneManager::Finalize() {
	finalRenderTex_.reset();
	scenes_.clear();
	lightGroup_.reset();
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
	MeshInstancingRenderer::SetLightGroup(lightGroup_.get());
	ModelManager::GetInstance()->SetLightGroup(lightGroup_.get());
	ParticleSystem::SetLightGroup(lightGroup_.get());

	SetSceneLayers(scenes_[currentSceneName_]->GetSceneLayers());

	GameObjectManager::AddObjectsToObjectsCopy();;
}


/// ===================================================
/// シーンのデバッグ
/// ===================================================

void SceneManager::Scene(ImGuiWindowFlags _imguiWindowFlags) {
	if(!ImGui::Begin("Scene", nullptr, _imguiWindowFlags)) {
		ImGui::End();
		return;
	}

	PlayControl();


	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 winSize = {
		max.x - min.x,
		max.y - min.y
	};

	///- 大きさの調整
	ImVec2 texSize = winSize;
	if(texSize.x <= texSize.y) {
		///- x優先
		texSize.y = (texSize.x / 16.0f) * 9.0f;
	} else {
		///- y優先
		float x = (texSize.y / 9.0f) * 16.0f;
		if(x < texSize.x) {
			texSize.x = x;
		} else {
			texSize.y = (texSize.x / 16.0f) * 9.0f;
		}
	}

	ImVec2 texPos = {
		winSize.x * 0.5f,
		winSize.y * 0.5f
	};

	texPos.y -= texSize.y / 2.0f;
	texPos.x -= texSize.x / 2.0f;

	texPos.x = std::max(texPos.x, min.x);
	texPos.y = std::max(texPos.y, min.y + 64.0f);

	
	/// scene gpu handle ptr
	auto renderTex = GetFinalRenderTex();
	ImTextureID sceneId = ImTextureID(renderTex->GetSrvGpuHandle().ptr);
	ImTextureID bgId = ImTextureID(TextureManager::GetInstance()->GetTexture("white2x2").GetGPUHandle().ptr);

	/// scene draw
	ImGui::SetCursorPos(texPos);
	ImGui::Image(bgId, texSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 1));
	ImGui::SetCursorPos(texPos);
	ImGui::Image(sceneId, texSize);

	ImGui::End();
}

void SceneManager::DebugScene(ImGuiWindowFlags _imguiWindowFlags) {
	if(!ImGui::Begin("Debug", nullptr, _imguiWindowFlags)) {
		ImGui::End();
		return;
	}

	PlayControl();

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 winSize = {
		max.x - min.x,
		max.y - min.y
	};

	///- 大きさの調整
	ImVec2 texSize = winSize;
	if(texSize.x <= texSize.y) {
		///- x優先
		texSize.y = (texSize.x / 16.0f) * 9.0f;
	} else {
		///- y優先
		float x = (texSize.y / 9.0f) * 16.0f;
		if(x < texSize.x) {
			texSize.x = x;
		} else {
			texSize.y = (texSize.x / 16.0f) * 9.0f;
		}
	}

	ImVec2 texPos = {
		winSize.x * 0.5f,
		winSize.y * 0.5f
	};

	texPos.y -= texSize.y / 2.0f;
	texPos.x -= texSize.x / 2.0f;

	texPos.x = std::max(texPos.x, min.x);
	texPos.y = std::max(texPos.y, min.y + 64.0f);

	sceneRectMin_ = { texPos.x + windowPos.x, texPos.y + windowPos.y };
	sceneRectMax_ = { texSize.x, texSize.y };

	/// scene gpu handle ptr
	auto renderTex = GetFinalRenderTex();
	ImTextureID sceneId = ImTextureID(renderTex->GetSrvGpuHandle().ptr);
	ImTextureID bgId = ImTextureID(TextureManager::GetInstance()->GetTexture("white2x2").GetGPUHandle().ptr);

	/// scene draw
	ImGui::SetCursorPos(texPos);
	ImGui::Image(bgId, texSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 1));
	ImGui::SetCursorPos(texPos);
	ImGui::Image(sceneId, texSize);

	ImGui::End();
}

void SceneManager::PlayControl() {
	ONE::WinApp* winApp = ONEngine::GetWinApps().at("Game").get();
	HWND hwnd = winApp->GetHWND();

	if(ImGui::Button("Play")) {
		/// game window create
		if(!IsWindowVisible(hwnd)) {
			winApp->ShowGameWindow();
		}

	}

	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("Stop")) {
		/// game window desctory
		if(IsWindowVisible(hwnd)) {
			winApp->DestoryGameWindow();
		}

	}
}
