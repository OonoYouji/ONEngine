#include "Engine/WindowManager/WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>();
	std::unique_ptr<DxManager>    dxManager      = std::make_unique<DxManager>();

	windowManager->Initialize();
	dxManager->Initialize();


	Window* window = windowManager->GenerateWindow(L"game", Vec2(1280, 720), WindowManager::WindowType::Main);

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