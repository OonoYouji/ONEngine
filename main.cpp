#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>

#include <ModelManager.h>
#include <Model.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Start();

	ONE::Logger::ConsolePrint("execution!!!\n");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();

	ModelManager* modelManager = ModelManager::GetInstance();

	winApp->Initialize();
	dxCommon->Initialize();

	modelManager->Initialize();

	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s\n", executionTime));

	
	std::unique_ptr<Model> model(std::make_unique<Model>());
	model->Initialize();

	while(!winApp->ProcessMessage()) {
		
		dxCommon->PreDraw();

		model->Draw();

		dxCommon->PostDraw();

	}


	model.reset();
	modelManager->Finalize();

	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}