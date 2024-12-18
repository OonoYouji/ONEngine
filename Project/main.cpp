#include "Core/ONEngine.h"

#include "Framework/GameFrameWork.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONEngine::Initialize(L"DirectXGame", false, false, 60u);

	std::unique_ptr<GameFrameWork> gameFrameWork(new GameFrameWork);
	gameFrameWork->Run();
	
	ONEngine::Finalize();
	return 0;
}