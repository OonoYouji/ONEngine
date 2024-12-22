
#include <Engine/WindowManager/WindowManager.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>();
	windowManager->Initialize();

	Window* window = windowManager->GenerateWindow(L"game", Vec2(1280, 720), WindowManager::WindowType::Main);
	Window* imgui  = windowManager->GenerateWindow(L"imgui", Vec2(1280, 720));

	while(true) {
		window->Update();


		/// 破棄されたら終了
		if(window->GetProcessMessage()) {
			break;
		}
	}


	windowManager->Finalize();

	return 0;
}