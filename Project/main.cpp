#include "Engine/WindowManager/WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<DxManager>     dxManager     = std::make_unique<DxManager>();
	std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(dxManager.get());

	windowManager->Initialize();
	dxManager->Initialize();


	Window* window = windowManager->GenerateWindow(L"game", Vec2(1280, 720), WindowManager::WindowType::Main);

	while(true) {
		window->Update();


		dxManager->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV)->BindToCommandList(
			dxManager->GetDxCommand()->GetCommandList()
		);
		
		window->PreDraw();
		window->PostDraw();

		dxManager->GetDxCommand()->CommandExecute();
		window->Present();
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