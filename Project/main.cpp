#include "Core/ONEngine.h"

#include "Framework/GameFrameWork.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONEngine::Initialize(L"LE2A_04_オオノ_ヨウジ", false, false, 60u);

	std::unique_ptr<GameFrameWork> gameFrameWork(new GameFrameWork);
	gameFrameWork->Run();
	
	ONEngine::Finalize();
	return 0;
}