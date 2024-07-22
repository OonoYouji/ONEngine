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

	modelManager->Initialize();
	modelManager->Load("MultiMaterial/MultiMaterial.obj");

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");

	Model* model = modelManager->GetModel("MultiMaterial/MultiMaterial.obj");
	Vec3 rotate{};

	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		cameraManager->Update();
		
		rotate.y += 1.0f / 64.0f;
		model->SetRotate(rotate);


		dxCommon->PreDraw();
		modelManager->PreDraw();


		modelManager->PostDraw();
		dxCommon->PostDraw();

	}


	cameraManager->Finalize();
	modelManager->Finalize();
	textureManager->Finalize();

	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}