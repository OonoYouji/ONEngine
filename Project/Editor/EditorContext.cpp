#include "EditorContext.h"
#include "Engine/Core/Application.h"

namespace Engine::Editor {

bool EditorContext::IsPlaying() const {
    return Engine::Core::Application::GetInstance().IsPlaying();
}

void EditorContext::Play() {
    Engine::Core::Application::GetInstance().Play();
}

void EditorContext::Stop() {
    Engine::Core::Application::GetInstance().Stop();
}

bool EditorContext::IsPaused() const {
    return Engine::Core::Application::GetInstance().IsPaused();
}

void EditorContext::SetPaused(bool paused) {
    Engine::Core::Application::GetInstance().SetPaused(paused);
}

} // namespace Engine::Editor
