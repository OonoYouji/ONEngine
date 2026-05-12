#pragma once

#include <Windows.h>
#include "Engine/Graphics/Core/ClusteredLightManager.h"
#include "Engine/Graphics/Core/GPUCullingManager.h"
#include <memory>
#include <functional>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Core/Timer.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Systems/TransformSystem.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include "Engine/ECS/Systems/CameraSystem.h"
#include "Engine/ECS/Systems/LightSystem.h"
#include "Engine/ECS/Systems/SkyboxSystem.h"
#include "Engine/ECS/Systems/TextSystem.h"
#include "Engine/ECS/Systems/ParticleSystem.h"
#include "Engine/ECS/Systems/AnimationSystem.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Core/RawInputService.h"
#include "Engine/Core/InputActions.h"
#include "Engine/Core/InputBinding.h"
#include "Engine/Core/InputMapper.h"

namespace Engine::Core {

///
/// エンジンのライフサイクルと実行を管理するアプリケーションクラス
///
class Application {
public:
    static Application& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new Application();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();
    void WaitForGPU();

    void SetEditorMode(bool enabled) { isEditorMode_ = enabled; }
    bool IsEditorMode() const { return isEditorMode_; }

    HWND GetHWND() const { return window_.GetHWND(); }

    ECS::Registry& GetRegistry() { return registry_; }

    RawInputService& GetRawInputService() { return rawInputService_; }
    ActionMap& GetGameActionMap() { return gameActionMap_; }

    void RegisterUICallback(const std::function<void()>& callback) { uiCallbacks_.push_back(callback); }

private:
    Application();
    ~Application();

    static Application* instance_;

    void Update(float dt);
    void Render();

private:
    Window window_;
    Timer timer_;
    
    std::vector<std::function<void()>> uiCallbacks_;
    
    // ECS
    ECS::Registry registry_;
    std::unique_ptr<ECS::TransformSystem> transformSystem_;
    std::unique_ptr<ECS::RenderSystem> renderSystem_;
    std::unique_ptr<ECS::CameraSystem> cameraSystem_;
    std::unique_ptr<ECS::LightSystem> lightSystem_;
    std::unique_ptr<ECS::SkyboxSystem> skyboxSystem_;
    std::unique_ptr<ECS::TextSystem> textSystem_;
    std::unique_ptr<ECS::ParticleSystem> particleSystem_;
    std::unique_ptr<ECS::AnimationSystem> animationSystem_;

    // Temp buffers
    std::unique_ptr<Graphics::StructuredBuffer> pointLightSB_;
    std::unique_ptr<Graphics::StructuredBuffer> spriteSB_;
    std::unique_ptr<Graphics::StructuredBuffer> textSB_;
    std::unique_ptr<Graphics::ClusteredLightManager> clusteredLightManager_;
    std::unique_ptr<Graphics::GPUCullingManager> gpuCullingManager_;

    // Scripting
    void(*updateDelegate_)() = nullptr;
    void(*shutdownDelegate_)() = nullptr;

    bool isEditorMode_ = false;

    RawInputService rawInputService_;
    ActionMap gameActionMap_;
    BindingTable gameBindingTable_;
    InputMapper inputMapper_;
};

} // namespace Engine::Core
