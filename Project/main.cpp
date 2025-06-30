/// engine
#include "Engine/Core/GameFramework/GameFramework.h"
#include "Engine/Core/Utility/Tools/Log.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifdef _DEBUG
	Console console;
#endif // _DEBUG

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName = L"TwoEngine",
		.windowSize = Vec2(1280, 720),
	});

	gameFramework->Run();

	return 0;
}