/// engine
#include "Engine/GameFramework/GameFramework.h"

/// game loop
#include "GameLoop/GameLoop.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkStartSetting{
		.windowName	= L"TwoEngine",
		.windowSize = Vec2(1280, 720),
		.gameLoop   = new GameLoop()
	});

	gameFramework->Run();

	gameFramework->Finalize();

	return 0;
}