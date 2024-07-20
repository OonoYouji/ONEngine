#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>

#include <ModelManager.h>
#include <CameraManager.h>
#include <TextureManager.h>

#include <GameCamera.h>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Start();

	ONE::Logger::ConsolePrint("execution!!!");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();

	ModelManager* modelManager = ModelManager::GetInstance();
	CameraManager* cameraManager = CameraManager::GetInstance();
	TextureManager* textureManager = TextureManager::GetInstance();

	winApp->Initialize();
	dxCommon->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");

	modelManager->Initialize();


	Model* model = new Model();
	model->Initialize();
	model->SetFillMode(kSolid);

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");


	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		cameraManager->Update();

		dxCommon->PreDraw();
		modelManager->PreDraw();



		modelManager->PostDraw();
		dxCommon->PostDraw();

	}


	modelManager->Finalize();
	textureManager->Finalize();

	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}