#include "Engine/Core/Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    Engine::Core::Application::CreateInstance();
    auto& app = Engine::Core::Application::GetInstance();

    if (!app.Initialize(hInstance, nCmdShow)) {
        return -1;
    }

    app.Run();
    app.Shutdown();
    Engine::Core::Application::DestroyInstance();

    return 0;
}
