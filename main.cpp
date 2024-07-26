#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>

#include <ImGuiManager.h>
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
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

	winApp->Initialize();
	dxCommon->Initialize();

	imGuiManager->Initialize(winApp, dxCommon);

	modelManager->Initialize();
	//modelManager->Load("MultiMaterial");

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");

	Model* model = modelManager->CreateCube();
	Vec3 rotate{};

	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		imGuiManager->BeginFrame();

		cameraManager->Update();
		
		rotate.y += 1.0f / 64.0f;
		model->SetRotate(rotate);


		dxCommon->PreDraw();
		modelManager->PreDraw();

		model->Draw();

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