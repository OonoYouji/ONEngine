/// engine
#include "Engine/GameFramework/GameFramework.h"

/// framework config
#include "GameLoop/GameLoop.h"
#include "Scene/Factory/SceneFactory.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName   = L"TwoEngine",
		.windowSize   = Vec2(1280, 720),
		.gameLoop     = new GameLoop()
	});

	gameFramework->Run();

	gameFramework->Finalize();

	return 0;
}