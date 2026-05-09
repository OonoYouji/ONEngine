#pragma once

#include <Windows.h>
#include "Engine/Graphics/Core/ClusteredLightManager.h"
#include "Engine/Graphics/Core/GPUCullingManager.h"
#include <memory>
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

namespace Engine::Core {

///
/// エンジンのライフサイクルと実行を管理するアプリケーションクラス
///
class Application {
public:
    static Application& GetInstance() {
        static Application instance;
        return instance;
    }

    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();

    ECS::Registry& GetRegistry() { return registry_; }

private:
    void Update(float dt);
    void Render();

private:
    Window window_;
    Timer timer_;
    
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

    // Rendering Resources
    std::unique_ptr<Graphics::StructuredBuffer> pointLightSB_;
    std::unique_ptr<Graphics::StructuredBuffer> spriteSB_;
    std::unique_ptr<Graphics::StructuredBuffer> textSB_;
    std::unique_ptr<Graphics::ClusteredLightManager> clusteredLightManager_;
    std::unique_ptr<Graphics::GPUCullingManager> gpuCullingManager_;

    // Scripting
    void(*updateDelegate_)(float) = nullptr;
    void(*shutdownDelegate_)() = nullptr;
};

} // namespace Engine::Core
