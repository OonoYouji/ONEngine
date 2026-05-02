///// engine
//#include "Engine/Core/GameFramework/GameFramework.h"
//#include "Engine/Core/Utility/Tools/Log.h"
//
//
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//	ONEngine::Console console;
//
//	std::unique_ptr<ONEngine::GameFramework> gameFramework = std::make_unique<ONEngine::GameFramework>();
//	gameFramework->Initialize(ONEngine::GameFrameworkConfig{
//		.windowName = L"TwoEngine",
//		.windowSize = ONEngine::Vector2::HD,
//	});
//
//	gameFramework->Run();
//
//	return 0;
//}


#include <Windows.h>
#include "Engine/Core/Window.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Core::Window window;
	window.Initialize(L"ThreeEngine", Engine::Math::Vector2Int{ 1280, 720 });
	
	while(true) {
		window.Update();



		if(window.GetIsProcessEnd()) {
			break;
		}
	}

	window.Shutdown();
	return 0;
}