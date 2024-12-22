
#include <Engine/WindowManager/WindowManager.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>();
	windowManager->Initialize();

	Window* window = windowManager->GenerateWindow(L"game", Vec2(1280, 720));

	while(true) {

	}


	windowManager->Finalize();

	return 0;
}