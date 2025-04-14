/// engine
#include "Engine/Core/GameFramework/GameFramework.h"
#include "Engine/Core/Utility/DebugTools/Log.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Console console;

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName = L"TwoEngine",
		.windowSize = Vec2(1280, 720),
	});


	gameFramework->Run();


	Assert(false, "GameFramework is not running");

	return 0;
}