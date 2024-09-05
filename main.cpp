#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDescriptor.h>
#include <FrameTimer.h>
#include <Input.h>

#include <SceneManager.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <TextureManager.h>
#include <AudioManager.h>
#include <ImGuiManager.h>
#include <CameraManager.h>
#include <GameObjectManager.h>
#include "Collision/CollisionManager.h"

#include <GameCamera.h>
#include <DebugCamera.h>

#include <RenderTextureManager.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Begin();

	ONE::Logger::ConsolePrint("execution!!!");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();
	Input* input = Input::GetInsatnce();

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

	winApp->Initialize();
	dxCommon->Initialize();

	input->Initialize(winApp);

	imGuiManager->Initialize(winApp, dxCommon);
	modelManager->Initialize();
	spriteManager->Initialize();
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");
	textureManager->Load("gameClear", "gameClear.png");
	textureManager->Load("Floor", "Floor.png");

	audioManager->Load("fanfare.wav");

	//modelManager->Load("Sphere");


	gameObjectManager->Initialize();

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera(gameCamera);

	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize();

	sceneManager->Initialize();

	renderTexManager->Initialize(dxCommon->GetDxCommand()->GetList(), dxCommon->GetDxDescriptor());
	renderTexManager->CreateRenderTarget("3dObject", 0);
	renderTexManager->CreateRenderTarget("frontSprite", 1);
	renderTexManager->CreateRenderTarget("ImGui", 2);

	renderTexManager->SetIsBlending("frontSprite", false);

	///- 実行までにかかった時間
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		frameTimer->Update();
		imGuiManager->BeginFrame();
		input->Begin();


		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================

		frameTimer->ImGuiDebug();
		gameObjectManager->ImGuiDebug();
		collisionManager->ImGuiDebug();

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
		modelManager->PreDraw();
		spriteManager->PreDraw();

		renderTexManager->BeginRenderTarget("3dObject");

		sceneManager->Draw();

#ifdef _DEBUG
		collisionManager->DrawHitBoxALL();
#endif // _DEBUG

		gameObjectManager->BackSpriteDraw();
		gameObjectManager->Draw();
		gameObjectManager->FrontSpriteDraw();

		modelManager->PostDraw();
		renderTexManager->EndRenderTarget("3dObject");

		
		renderTexManager->BeginRenderTarget("frontSprite");
		spriteManager->PostDraw();
		renderTexManager->EndRenderTarget("frontSprite");

		renderTexManager->EndFrame();

		renderTexManager->BeginRenderTarget("ImGui");
		imGuiManager->EndFrame();
		renderTexManager->EndRenderTarget("ImGui");

	
		dxCommon->PostDraw(renderTexManager->GetFinalRenderTexture()->GetRenderTexResource());
	}


	renderTexManager->Finalize();
	sceneManager->Finalize();
	cameraManager->Finalize();
	gameObjectManager->Finalize();

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