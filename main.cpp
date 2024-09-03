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
#include <AudioManager.h>
#include <ImGuiManager.h>
#include <CameraManager.h>
#include <GameObjectManager.h>

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
	AudioManager* audioManager = AudioManager::GetInstance();
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();
	CameraManager* cameraManager = CameraManager::GetInstance();
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();

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


	gameObjectManager->Initialize();

	GameCamera* gameCamera = new GameCamera();
	gameCamera->Initialize();
	cameraManager->SetMainCamera("GameCamera");

#ifdef _DEBUG
	std::unique_ptr<Scene_Debug> debugScene(new Scene_Debug());
	debugScene->Initialize();
#endif // _DEBUG

	sceneManager->Initialize();

	///- 実行までにかかった時間
	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", executionTime));


	while(!winApp->ProcessMessage()) {

		imGuiManager->BeginFrame();
		input->Begin();


		/// ====================================
		/// ↓ 更新処理に移る
		/// ====================================

		gameObjectManager->ImGuiDebug();

		sceneManager->Update();
		
		/// 更新1
		gameObjectManager->Update();
		/// 当たり判定処理

		/// 更新2
		gameObjectManager->LastUpdate();

		audioManager->Update();

		/// ====================================
		/// ↓ 描画処理に移る
		/// ====================================

		dxCommon->PreDraw();
		sceneManager->PreDraw();
		modelManager->PreDraw();
		spriteManager->PreDraw();

		sceneManager->Draw();

		modelManager->PostDraw();
		spriteManager->PostDraw();



		imGuiManager->EndFrame();
		sceneManager->PostDraw();
		dxCommon->PostDraw();

	}

#ifdef _DEBUG
	debugScene.reset();
#endif // _DEBUG


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