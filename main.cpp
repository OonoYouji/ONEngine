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

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
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



	winApp->Initialize();
	dxCommon->Initialize();

	input->Initialize(winApp);

	imGuiManager->Initialize(winApp, dxCommon);
	modelManager->Initialize();
	spriteManager->Initialize();
	lineDrawer2d->Initialize();
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");
	textureManager->Load("gameClear", "gameClear.png");
	textureManager->Load("Floor", "Floor.png");
	textureManager->Load("TVUV", "../Models/TV/TVUV.png");
	textureManager->Load("Heart", "../Models/Heart/Heart.png");
	//textureManager->Load("Hand", "../Models/Hand/Hand.png");

	audioManager->Load("fanfare.wav");

	renderTexManager->Initialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor());

	bool imguiIsBlending = true;
	renderTexManager->CreateRenderTarget("ImGui", 0, { 0,0,0,0 });
	renderTexManager->SetIsBlending("ImGui", imguiIsBlending);

	Bloom::StaticInitialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor(), 2);



	gameObjectManager->Initialize();

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera(gameCamera);

	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize();

	DirectionalLight* directionalLight = new DirectionalLight();
	directionalLight->Initialize();
	modelManager->SetDirectionalLight(directionalLight);


	sceneManager->Initialize();


	std::vector<std::unique_ptr<SceneLayer>> layers;
	layers.resize(2);
	{
		std::string names[2]{ "monitor", "game" };
		BaseCamera* pCameras[2]{ gameCamera, debugCamera };
		for(uint8_t i = 0; i < layers.size(); ++i) {
			layers[i].reset(new SceneLayer);
			layers[i]->Initialize(names[i], pCameras[i]);
		}
	}


#ifdef _DEBUG
	std::unique_ptr<RenderTexture> debugFinalRenderTexture(new RenderTexture);
	debugFinalRenderTexture->Initialize(
		Vec4(0, 0, 0, 0),
		dxCommon->GetDxCommand()->GetList(),
		dxCommon->GetDxDescriptor()
	);
#endif // _DEBUG


	///- 実行までにかかった時間
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		worldTime->Update();
		imGuiManager->BeginFrame();
		input->Begin();


		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================

		worldTime->ImGuiDebug();
		gameObjectManager->ImGuiDebug();
		collisionManager->ImGuiDebug();
		renderTexManager->ImGuiDebug();
		for(auto& layer : layers) {
			layer->ImGuiDebug();
		}

#ifdef _DEBUG
		if(Input::TriggerKey(KeyCode::F5)) {
			imguiIsBlending = !imguiIsBlending;
			renderTexManager->SetIsBlending("ImGui", imguiIsBlending);
		}
#endif // _DEBUG


		cameraManager->Update();

		sceneManager->Update();

		/// 更新1
		gameObjectManager->Update();
		/// 当たり判定処理
		collisionManager->Update();
		/// 更新2
		gameObjectManager->LastUpdate();

		audioManager->Update();

		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		dxCommon->PreDraw();

		for(auto& layer : layers) {
			layer->Draw();
		}

		renderTexManager->EndFrame();

#ifdef _DEBUG
		RenderTextureManager::CreateBlendRenderTexture(
			{ layers.back()->GetFinalRenderTexture() , renderTexManager->GetRenderTexture("ImGui") },
			debugFinalRenderTexture.get()
		);

		renderTexManager->BeginRenderTarget("ImGui");
		imGuiManager->EndFrame();
		renderTexManager->EndRenderTarget("ImGui");

		dxCommon->PostDraw(debugFinalRenderTexture.get());
#else
		dxCommon->PostDraw(layers.back()->GetFinalRenderTexture());
#endif // _DEBUG

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