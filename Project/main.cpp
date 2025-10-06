/// engine
#include "Engine/Core/GameFramework/GameFramework.h"
#include "Engine/Core/Utility/Tools/Log.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Console console;

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName = L"TwoEngine",
		.windowSize = Vector2(1280, 720),
	});

	gameFramework->Run();

	return 0;
}