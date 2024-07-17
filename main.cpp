#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>
#include <DxCommon.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::Logger::ConsolePrint("execution!!!\n");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	winApp->Initialize();

	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();
	dxCommon->Initialize();

	while(!winApp->ProcessMessage()) {

	}

	dxCommon->Finalize();
	winApp->Finalize();

	return 0;
}