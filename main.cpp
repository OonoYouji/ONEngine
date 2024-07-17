#include <Windows.h>

#include <WinApp.h>
#include <Logger.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ONE::Logger::ConsolePrint("execution!!!\n");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	winApp->Initialize();

	while(!winApp->ProcessMessage()) {

	}

	winApp->Finalize();

	return 0;
}