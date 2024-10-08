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




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	/// 起動速度を計算するため
	auto currentTime = std::chrono::high_resolution_clock::now();;
	ONE::Logger::ConsolePrint("execution!!!");

	Input*			input		= Input::GetInsatnce();
	Time*			time		= Time::GetInstance();
	std::unique_ptr<FrameFixation> frameFixation = nullptr;

	SceneManager*			sceneManager		= SceneManager::GetInstance();
	ModelManager*			modelManager		= ModelManager::GetInstance();
	SpriteManager*			spriteManager		= SpriteManager::GetInstance();
	TextureManager*			textureManager		= TextureManager::GetInstance();
	AudioManager*			audioManager		= AudioManager::GetInstance();
	ImGuiManager*			imGuiManager		= ImGuiManager::GetInstance();
	CameraManager*			cameraManager		= CameraManager::GetInstance();
	GameObjectManager*		gameObjectManager	= GameObjectManager::GetInstance();
	CollisionManager*		collisionManager	= CollisionManager::GetInstance();
	RenderTextureManager*	renderTexManager	= RenderTextureManager::GetInstance();
	Line2D*					line2d				= Line2D::GetInstance();


	ONEngine::Initialize(L"DirectXGame", false);

	frameFixation.reset(new FrameFixation);
	frameFixation->Initialize(false);

#ifdef _DEBUG
	imGuiManager->Initialize(ONEngine::GetWinApps().at("Debug").get(), ONEngine::GetDxCommon());
#endif // _DEBUG

	modelManager->Initialize();
	spriteManager->Initialize();
	line2d->Initialize();
	Line3D::SInitialize(ONEngine::GetDxCommon()->GetDxCommand()->GetList());
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("white2x2", "white2x2.png");


	/// render texture imgui用を作成
	renderTexManager->Initialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(), 
		ONEngine::GetDxCommon()->GetDxDescriptor()
	);
	renderTexManager->CreateRenderTarget("ImGui", 0, { 0,0,0,0 });

	/// bloomエフェクトの初期化
	Bloom::StaticInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor(), 2
	);

	ParticleSystem::SInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor()
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


#ifdef _DEBUG
	/// debug 用 render texture の初期化
	std::unique_ptr<RenderTexture> debugFinalRenderTexture(new RenderTexture);
	debugFinalRenderTexture->Initialize(
		Vec4(0, 0, 0, 0),
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()->GetDxDescriptor()
	);
#endif // _DEBUG

	/// window mode や imgui の表示設定の初期化
	ONEngine::GetMainWinApp()->SetIsFullScreen(false); /// ? full screen : window mode
	uint8_t drawLayerIndex = 0u;
	renderTexManager->SetIsBlending("ImGui", true);


	///- 実行までにかかった時間
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - currentTime;
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", duration.count() / 1000.0f));

	time->Update();


	while(ONEngine::IsRunning()) {

		/// 終了命令がでたのでループを抜ける
		if(!sceneManager->GetIsRunning()) {
			break;
		}


		time->Update();
		input->Update();
#ifdef _DEBUG
		imGuiManager->BeginFrame();
#endif // _DEBUG
		ONEngine::Update();



		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================


		cameraManager->Update();
		sceneManager->Update();

		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		auto& winApps = ONEngine::GetWinApps();
		for(auto& win : winApps) {
			win.second->PreDraw();
		}

		sceneManager->Draw();

#ifdef _DEBUG
		renderTexManager->BeginRenderTarget("ImGui");
		imGuiManager->EndFrame();
		renderTexManager->EndRenderTarget("ImGui");

		winApps.at("Debug")->PostDraw(renderTexManager->GetRenderTexture("ImGui"));
#endif // _DEBUG

		winApps.at("Game")->PostDraw(sceneManager->GetSceneLayer(drawLayerIndex)->GetRenderTexture());

		ONEngine::GetDxCommon()->CommandExecution();
		for(auto& win : winApps) {
			win.second->Present();
		}
		ONEngine::GetDxCommon()->GetDxCommand()->Reset();

		frameFixation->Fixation();

	}


	if(!ONEngine::GetDxCommon()->IsGpuExeEnded()) {
		assert(false);
	}


#ifdef _DEBUG
	debugFinalRenderTexture.reset();
#endif // _DEBUG

	renderTexManager->Finalize();
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
#ifdef _DEBUG
	imGuiManager->Finalize();
#endif // _DEBUG
	input->Finalize();
	ONEngine::Finalize();

	return 0;
}