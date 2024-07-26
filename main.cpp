#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>

#include <SceneManager.h>
#include <ModelManager.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <CameraManager.h>

#include <GameCamera.h>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Start();

	ONE::Logger::ConsolePrint("execution!!!");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();

	SceneManager* sceneManager = SceneManager::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	TextureManager* textureManager = TextureManager::GetInstance();
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();
	CameraManager* cameraManager = CameraManager::GetInstance();

	winApp->Initialize();
	dxCommon->Initialize();
	imGuiManager->Initialize(winApp, dxCommon);
	modelManager->Initialize();
	sceneManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");


	///- 実行までにかかった時間
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		imGuiManager->BeginFrame();

		cameraManager->Update();
		sceneManager->Update();


		dxCommon->PreDraw();
		modelManager->PreDraw();

		sceneManager->Draw();

		modelManager->PostDraw();
		imGuiManager->EndFrame();
		dxCommon->PostDraw();

	}


	cameraManager->Finalize();
	modelManager->Finalize();
	textureManager->Finalize();

	imGuiManager->Finalize();
	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}