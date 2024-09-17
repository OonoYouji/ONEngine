#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDescriptor.h>
#include <FrameTimer.h>
#include <WorldTime.h>
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
#include "Collision/CollisionManager.h"

#include <GameCamera.h>
#include <DebugCamera.h>
#include <Light/DirectionalLight.h>

#include <RenderTextureManager.h>
#include <SceneLayer/SceneLayer.h>
#include <Bloom/Bloom.h>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Begin();

	ONE::Logger::ConsolePrint("execution!!!");

	std::unique_ptr<ONE::WinApp> winApp = nullptr;
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();
	Input* input = Input::GetInsatnce();
	WorldTime* worldTime = WorldTime::GetInstance();

	SceneManager* sceneManager = SceneManager::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();
	TextureManager* textureManager = TextureManager::GetInstance();
	AudioManager* audioManager = AudioManager::GetInstance();
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();
	CameraManager* cameraManager = CameraManager::GetInstance();
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();
	CollisionManager* collisionManager = CollisionManager::GetInstance();
	RenderTextureManager* renderTexManager = RenderTextureManager::GetInstance();
	LineDrawer2D* lineDrawer2d = LineDrawer2D::GetInstance();



	winApp.reset(new ONE::WinApp);
	winApp->Initialize(L"DirectXGame");

	dxCommon->Initialize(winApp.get());

	input->Initialize(winApp.get());

	imGuiManager->Initialize(winApp.get(), dxCommon);
	modelManager->Initialize();
	spriteManager->Initialize();
	lineDrawer2d->Initialize();
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");


	/// render texture imgui用を作成
	renderTexManager->Initialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor());
	renderTexManager->CreateRenderTarget("ImGui", 0, { 0,0,0,0 });

	/// bloomエフェクトの初期化
	Bloom::StaticInitialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor(), 2);

	/// game object manager の初期化
	gameObjectManager->Initialize();

	/// camera の初期化
	GameCamera* monitorCamera = new GameCamera("MonitorCamera");
	monitorCamera->Initialize();
	monitorCamera->SetPosition({ -2.221f, 3.245f, -27.257f });
	monitorCamera->SetRotate({ 0.0f, 0.215f, 0.0f });
	monitorCamera->BaseUpdate();

	GameCamera* gameCamera = new GameCamera("GameCamera");
	gameCamera->Initialize();
	gameCamera->SetPosition({ 0.0f, 0.2f, -15.0f });
	gameCamera->SetRotate({ 0.0f, -0.12f, 0.0f });
	gameCamera->BaseUpdate();
	cameraManager->SetMainCamera(gameCamera);

	/// {0.0f, 0.2f, -15.0f}, { 0.0f, -0.12f, 0.0f }


	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize();
	debugCamera->SetPosition({ -1.48f, 0.9f, -14.16f });
	debugCamera->SetRotate({ 0.066f, 0.0f, 0.0f });
	debugCamera->BaseUpdate();

	/// light の初期化
	DirectionalLight* directionalLight = new DirectionalLight();
	directionalLight->Initialize();
	modelManager->SetDirectionalLight(directionalLight);


	/// layer の初期化
	std::vector<std::unique_ptr<SceneLayer>> layers;
	layers.resize(1);
	{
		std::string names[2]{ "monitor", "game" };
		//BaseCamera* pCameras[2]{ monitorCamera, debugCamera };
		BaseCamera* pCameras[2]{ debugCamera, gameCamera };
		for(uint8_t i = 0; i < layers.size(); ++i) {
			layers[i].reset(new SceneLayer);
			layers[i]->Initialize(names[i], pCameras[i]);
		}
	}

	///////////////////////////////////////////////////////////////////////
	/// scene manager の初期化	: 初期化時のシーンをここで決定
	///////////////////////////////////////////////////////////////////////

	sceneManager->SetSceneLayers({ layers[0].get() });
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
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));

	worldTime->Update();


	while(!winApp->ProcessMessage()) {

		/// 終了命令がでたのでループを抜ける
		if(!sceneManager->GetIsRunning()) {
			break;
		}


		worldTime->Update();
		imGuiManager->BeginFrame();
		input->Begin();


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
		if(Input::TriggerKey(KeyCode::Alpha1)) {
			drawLayerIndex = 0;
		}
		if(Input::TriggerKey(KeyCode::Alpha2)) {
			drawLayerIndex = 1;
		}

		/// ImGuiの表示
		worldTime->ImGuiDebug();
		gameObjectManager->ImGuiDebug();
		collisionManager->ImGuiDebug();
		renderTexManager->ImGuiDebug();
		sceneManager->ImGuiDebug();
		for(auto& layer : layers) {
			layer->ImGuiDebug();
		}

#endif // _DEBUG

		cameraManager->Update();

		/// game object の更新をしている
		sceneManager->Update();


		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		dxCommon->PreDraw();

		for(auto& layer : layers) {
			layer->Draw();
		}

		renderTexManager->EndFrame();

#ifdef _DEBUG
		if(imguiIsBlending) {
			RenderTextureManager::CreateBlendRenderTexture(
				{ layers[drawLayerIndex]->GetFinalRenderTexture() , renderTexManager->GetRenderTexture("ImGui") },
				debugFinalRenderTexture.get()
			);
		}

		renderTexManager->BeginRenderTarget("ImGui");
		imGuiManager->EndFrame();
		renderTexManager->EndRenderTarget("ImGui");
		if(imguiIsBlending) {
			dxCommon->PostDraw(debugFinalRenderTexture.get());
		} else {
			dxCommon->PostDraw(layers[drawLayerIndex]->GetFinalRenderTexture());
		}
#else
		dxCommon->PostDraw(layers.back()->GetFinalRenderTexture());
#endif // _DEBUG

	}


	if(!dxCommon->IsGpuExeEnded()) {
		assert(false);
	}


#ifdef _DEBUG
	debugFinalRenderTexture.reset();
#endif // _DEBUG
	layers.clear();

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