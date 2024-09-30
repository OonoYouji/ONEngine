#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDescriptor.h>
#include <Time/Time.h>
#include <FrameFixation/FrameFixation.h>
#include <Input.h>

#include <ModelManager.h>
#include <SpriteManager.h>
#include <TextureManager.h>
#include <AudioManager.h>
#include <LineDrawer2D/LineDrawer2D.h>

#include <SceneManager.h>
#include <ImGuiManager.h>
#include <CameraManager.h>
#include <GameObjectManager.h>
//#include "Collision/CollisionManager.h"

#include <GameCamera.h>
#include <DebugCamera.h>
#include <Light/DirectionalLight.h>

#include <RenderTextureManager.h>
#include <SceneLayer/SceneLayer.h>
#include <Bloom/Bloom.h>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	/// 起動速度を計算するため
	auto currentTime = std::chrono::high_resolution_clock::now();;


	ONE::Logger::ConsolePrint("execution!!!");

	std::unique_ptr<ONE::WinApp> winApp = nullptr;
	ONE::DxCommon*	dxCommon	= ONE::DxCommon::GetInstance();
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
	//CollisionManager* collisionManager = CollisionManager::GetInstance();
	RenderTextureManager*	renderTexManager	= RenderTextureManager::GetInstance();
	LineDrawer2D*			lineDrawer2d		= LineDrawer2D::GetInstance();



	winApp.reset(new ONE::WinApp);
	winApp->Initialize(L"DirectXGame");

	dxCommon->Initialize(winApp.get());

	input->Initialize(winApp.get());

	frameFixation.reset(new FrameFixation);
	frameFixation->Initialize(true);


	imGuiManager->Initialize(winApp.get(), dxCommon);
	modelManager->Initialize();
	spriteManager->Initialize();
	lineDrawer2d->Initialize();
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("white2x2", "white2x2.png");


	/// render texture imgui用を作成
	renderTexManager->Initialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor());
	renderTexManager->CreateRenderTarget("ImGui", 0, { 0,0,0,0 });

	/// bloomエフェクトの初期化
	Bloom::StaticInitialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor(), 2);

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
		dxCommon->GetDxCommand()->GetList(),
		dxCommon->GetDxDescriptor()
	);
#endif // _DEBUG

	/// window mode や imgui の表示設定の初期化
	winApp->SetIsFullScreen(false); /// ? full screen : window mode
	uint8_t drawLayerIndex = 0;	/// game || monitor
	bool imguiIsBlending = true;
	renderTexManager->SetIsBlending("ImGui", imguiIsBlending);

	///- 実行までにかかった時間
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - currentTime;
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", duration.count() / 1000.0f));

	time->Update();


	while(!winApp->ProcessMessage()) {

		/// 終了命令がでたのでループを抜ける
		if(!sceneManager->GetIsRunning()) {
			break;
		}


		time->Update();
		imGuiManager->BeginFrame();
		input->Update();


		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================

		/// フルスクリーンの切り替え
		if(Input::TriggerKey(KeyCode::F11)) {
			winApp->SetIsFullScreen(!winApp->GetIsFullScreen());
		}

#ifdef _DEBUG

		if(Input::TriggerKey(KeyCode::F5)) {
			imguiIsBlending = !imguiIsBlending;
		}
		
		/// ImGuiの表示
		input->ImGuiDebug();
		time->ImGuiDebug();
		frameFixation->ImGuiDebug();
		gameObjectManager->ImGuiDebug();
		//collisionManager->ImGuiDebug();
		renderTexManager->ImGuiDebug();
		sceneManager->ImGuiDebug();
		

#endif // _DEBUG

		cameraManager->Update();

		/// game object の更新をしている
		sceneManager->Update();


		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		dxCommon->PreDraw();

		sceneManager->Draw();

		renderTexManager->EndFrame();

#ifdef _DEBUG
		if(imguiIsBlending) {
			RenderTextureManager::CreateBlendRenderTexture(
				{ sceneManager->GetSceneLayer(drawLayerIndex)->GetFinalRenderTexture() , renderTexManager->GetRenderTexture("ImGui") },
				debugFinalRenderTexture.get()
			);
		}

		renderTexManager->BeginRenderTarget("ImGui");
		imGuiManager->EndFrame();
		renderTexManager->EndRenderTarget("ImGui");

		frameFixation->Fixation();
		if(imguiIsBlending) {
			dxCommon->PostDraw(debugFinalRenderTexture.get());
		} else {
			dxCommon->PostDraw(sceneManager->GetSceneLayer(drawLayerIndex)->GetFinalRenderTexture());
		}
#else
		frameFixation->Fixation();
		dxCommon->PostDraw(sceneManager->GetSceneLayer(drawLayerIndex)->GetFinalRenderTexture());
#endif // _DEBUG

	}


	if(!dxCommon->IsGpuExeEnded()) {
		assert(false);
	}


#ifdef _DEBUG
	debugFinalRenderTexture.reset();
#endif // _DEBUG

	renderTexManager->Finalize();
	Bloom::StaticFinalize();

	sceneManager->Finalize();
	cameraManager->Finalize();
	gameObjectManager->Finalize();

	lineDrawer2d->Finalize();
	audioManager->Finalize();
	spriteManager->Finalize();
	modelManager->Finalize();

	textureManager->Finalize();
	imGuiManager->Finalize();
	input->Finalize();
	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}