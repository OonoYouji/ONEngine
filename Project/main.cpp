/// engine
#include "Engine/GameFramework/GameFramework.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName = L"TwoEngine",
		.windowSize = Vec2(1280, 720),
	});


	gameFramework->Run();

	return 0;
}