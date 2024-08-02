#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>
#include <Input.h>

#include <SceneManager.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <CameraManager.h>

#include <GameCamera.h>
#include <Scene_Debug.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Start();

	ONE::Logger::ConsolePrint("execution!!!");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();
	Input* input = Input::GetInsatnce();

	SceneManager* sceneManager = SceneManager::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();
	TextureManager* textureManager = TextureManager::GetInstance();
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();
	CameraManager* cameraManager = CameraManager::GetInstance();

	winApp->Initialize();
	dxCommon->Initialize();

	input->Initialize(winApp);

	imGuiManager->Initialize(winApp, dxCommon);
	modelManager->Initialize();
	spriteManager->Initialize();
	sceneManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("monsterBall", "monsterBall.png");
	textureManager->Load("gameClear", "gameClear.png");
	

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");

#ifdef _DEBUG
	std::unique_ptr<Scene_Debug> debugScene(new Scene_Debug());
	debugScene->Initialize();
#endif // _DEBUG

	///- 実行までにかかった時間
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		imGuiManager->BeginFrame();
		input->Begin();


		cameraManager->Update();
		sceneManager->Update();


		dxCommon->PreDraw();
		sceneManager->PreDraw();
		modelManager->PreDraw();
		spriteManager->PreDraw();

		sceneManager->Draw();

		modelManager->PostDraw();
		spriteManager->PostDraw();

		
#ifdef _DEBUG
		debugScene->Draw();
#endif // _DEBUG

		sceneManager->PostDraw();
		sceneManager->SceneDraw();

		dxCommon->SetRenderTarget();
		imGuiManager->EndFrame();
		dxCommon->PostDraw();

	}


	sceneManager->Finalize();
	spriteManager->Finalize();
	cameraManager->Finalize();
	modelManager->Finalize();
	textureManager->Finalize();

	imGuiManager->Finalize();
	input->Finalize();
	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}