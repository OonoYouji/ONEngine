#include <Windows.h>

#include <Core/ONEngine.h>

#include <iostream>
#include <thread>

#include <WindowManager/WinApp.h>
#include <LoggingManager/Logger.h>
#include <GraphicManager/GraphicsEngine/DirectX12/DxCommon.h>
#include <GraphicManager/GraphicsEngine/DirectX12/DxCommand.h>
#include <GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h>
#include <FrameManager/Time.h>
#include <FrameManager/FrameFixation.h>
#include <Library/Input/Input.h>

#include <GraphicManager/ModelManager/ModelManager.h>
#include <GraphicManager/TextureManager/TextureManager.h>
#include <GraphicManager/Drawer/Sprite/SpriteManager.h>
#include <GraphicManager/Drawer/LineDrawer/Line2D.h>
#include <GraphicManager/Drawer/LineDrawer/Line3D.h>
#include <AudioManager/AudioManager.h>

#include <Scenes/Manager/SceneManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <Objects/Camera/Manager/CameraManager.h>
#include <GameObjectManager/GameObjectManager.h>
#include <Library/CollisionManager/CollisionManager.h>

#include <Objects/Camera/GameCamera.h>
#include <Objects/Camera/DebugCamera.h>
#include <GraphicManager/Light/DirectionalLight.h>

#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"
#include "GraphicManager/SceneLayer/SceneLayer.h"
#include "GraphicManager/PostEffect/Bloom/Bloom.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"


/// custom class
#include "CustomComponents/EarthRenderer/EarthRenderer.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	/// 起動速度を計算するため
	auto currentTime = std::chrono::high_resolution_clock::now();;
	ONE::Logger::ConsolePrint("execution!!!");

	std::unique_ptr<FrameFixation> frameFixation = nullptr;

	SceneManager*			sceneManager		= SceneManager::GetInstance();
	ModelManager*			modelManager		= ModelManager::GetInstance();
	SpriteManager*			spriteManager		= SpriteManager::GetInstance();
	TextureManager*			textureManager		= TextureManager::GetInstance();
	AudioManager*			audioManager		= AudioManager::GetInstance();
	CameraManager*			cameraManager		= CameraManager::GetInstance();
	GameObjectManager*		gameObjectManager	= GameObjectManager::GetInstance();
	CollisionManager*		collisionManager	= CollisionManager::GetInstance();
	Line2D*					line2d				= Line2D::GetInstance();


	ONEngine::Initialize(L"DirectXGame", false, false, 60u);

	modelManager->Initialize();
	spriteManager->Initialize();
	line2d->Initialize();
	Line3D::SInitialize(ONEngine::GetDxCommon()->GetDxCommand()->GetList());
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("white2x2", "white2x2.png");


	/// bloomエフェクトの初期化
	Bloom::StaticInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor(), 2
	);

	ParticleSystem::SInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor()
	);


	/// custom class initializing
	EarthRenderer::SInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor(),
		128u		/// 128体エンティティを出せる
	);


	/// game object manager の初期化
	gameObjectManager->Initialize();


	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize();
	debugCamera->SetPosition({ -1.48f, 0.9f, -14.16f });
	debugCamera->SetRotate({ 0.066f, 0.0f, 0.0f });
	debugCamera->BaseUpdate();


	///////////////////////////////////////////////////////////////////////
	/// scene manager の初期化	: 初期化時のシーンをここで決定
	///////////////////////////////////////////////////////////////////////
	sceneManager->Initialize(SCENE_ID::GAME);




	///- 実行までにかかった時間
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - currentTime;
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", duration.count() / 1000.0f));


	while(ONEngine::IsRunning()) {

		/// 終了命令がでたのでループを抜ける
		if(!sceneManager->GetIsRunning()) {
			break;
		}

		ONEngine::Update();



		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================


		cameraManager->Update();
		sceneManager->Update();

		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		
		ONEngine::PreDraw();

		sceneManager->Draw();

		ONEngine::PostDraw();

	}


	if(!ONEngine::GetDxCommon()->IsGpuExeEnded()) {
		assert(false);
	}


	EarthRenderer::SFinalize();;
	Bloom::StaticFinalize();
	ParticleSystem::SFinalize();

	sceneManager->Finalize();
	cameraManager->Finalize();
	gameObjectManager->Finalize();

	Line3D::SFinalize();
	line2d->Finalize();
	audioManager->Finalize();
	spriteManager->Finalize();
	modelManager->Finalize();

	textureManager->Finalize();
	ONEngine::Finalize();

	return 0;
}