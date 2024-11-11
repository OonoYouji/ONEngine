/// engine
#include "Core/ONEngine.h"
#include "LoggingManager/Logger.h"
/// game
#include "Scenes/Manager/SceneManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	/// 起動速度を計算する
	std::chrono::steady_clock::time_point start, end;
	start = std::chrono::high_resolution_clock::now();;
	ONE::Logger::ConsolePrint("execution!!!");

	/// engine, game initialize
	SceneManager* sceneManager = SceneManager::GetInstance();
	ONEngine::Initialize(L"DirectXGame", false, false, 60u);
	sceneManager->Initialize(SCENE_ID::GAME); /// 初期化時のシーンをここで決定

	/// 実行までにかかった時間
	end = std::chrono::high_resolution_clock::now();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s", (end - start).count() / 1000.0f));

	/// main loop
	while(ONEngine::GetIsRunning()) {
		/// ↓ 更新処理に移る
		ONEngine::Update();
		sceneManager->Update();

		/// ↓ 描画処理に移る
		ONEngine::PreDraw();
		sceneManager->Draw();
		ONEngine::PostDraw();
	}
	
	ONEngine::Finalize();
	return 0;
}