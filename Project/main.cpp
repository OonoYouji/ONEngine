#include "Core/ONEngine.h"
#include "Core/GameFrameWork.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONEngine::Initialize(L"2204_グスタフの巨人", false, false, 60u);

	std::unique_ptr<GameFrameWork> gameFrameWork(new GameFrameWork);
	gameFrameWork->Run();
	
	ONEngine::Finalize();
	return 0;
}