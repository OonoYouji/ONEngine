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
#include "Engine/Graphics/Core/GraphicsEngine.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Core::Window window;
	window.Initialize(L"ThreeEngine", Engine::Math::Vector2Int{ 1280, 720 });

	Engine::Graphics::GraphicsEngine& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
	graphicsEngine.Initialize();

	while(true) {
		window.Update();


		if(window.GetIsProcessEnd()) {
			break;
		}
	}

	graphicsEngine.Shutdown();
	window.Shutdown();

	return 0;
}