#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>
#include <FrameTimer.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::FrameTimer* frameTimer = ONE::FrameTimer::GetInstance();
	frameTimer->Start();

	ONE::Logger::ConsolePrint("execution!!!\n");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();


	winApp->Initialize();
	dxCommon->Initialize();

	float executionTime = frameTimer->End();
	ONE::Logger::ConsolePrint(std::format("ExecutionTime: {}s\n", executionTime));


	while(!winApp->ProcessMessage()) {

		dxCommon->PreDraw();
		dxCommon->PostDraw();

	}


	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}