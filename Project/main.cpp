#include "Engine/WindowManager/WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<DxManager>     dxManager     = std::make_unique<DxManager>();
	std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(dxManager.get());

	windowManager->Initialize();
	dxManager->Initialize();

	Window* window = windowManager->GenerateWindow(L"game", Vec2(1280, 720), WindowManager::WindowType::Main);
	windowManager->GenerateWindow(L"imgui", Vec2(1280, 720));
	windowManager->GenerateWindow(L"test", Vec2(640, 360));

	while(true) {
		windowManager->Update();


		dxManager->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV)->BindToCommandList(
			dxManager->GetDxCommand()->GetCommandList()
		);
		
		windowManager->PreDraw();
		windowManager->PostDraw();

		dxManager->GetDxCommand()->CommandExecute();
		windowManager->Present();
		dxManager->GetDxCommand()->CommandReset();

		/// 破棄されたら終了
		if(window->GetProcessMessage()) {
			break;
		}
	}

	dxManager->GetDxCommand()->WaitForGpuComplete();

	windowManager->Finalize();
	dxManager->Finalize();

	return 0;
}