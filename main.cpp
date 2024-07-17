#include <Windows.h>

#include <WinApp.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	OutputDebugStringA("ONE : Hellow\n");

	ONE::WinApp* winApp = ONE::WinApp::GetInstance();
	winApp->Initialize();

	while(!winApp->ProcessMessage()) {

	}

	winApp->Finalize();

	return 0;
}