/// engine
#include "Engine/Core/GameFramework/GameFramework.h"
#include "Engine/Core/Utility/Tools/Log.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ONEngine::Console console;

	std::unique_ptr<ONEngine::GameFramework> gameFramework = std::make_unique<ONEngine::GameFramework>();
	gameFramework->Initialize(ONEngine::GameFrameworkConfig{
		.windowName = L"TwoEngine",
		.windowSize = ONEngine::Vector2(1280, 720),
	});

	gameFramework->Run();

	return 0;
}