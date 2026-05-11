#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>
#include "Engine/Graphics/Utils/ComPtr.h"
#include "Engine/Core/Math/Math.h"
#include "Engine/Graphics/Core/RenderContext.h"

namespace Engine::Graphics {

class RenderDevice;
class StructuredBuffer;

///
/// デバッグ描画（ライン、グリッドなど）を管理するクラス
///
class DebugRenderer {
public:
    static constexpr uint32_t kMaxVertices = 1000000;

    static DebugRenderer& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new DebugRenderer();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    void DrawLine(const Engine::Math::Vector3& start, const Engine::Math::Vector3& end, const Engine::Math::Vector4& color);
    void DrawSphere(const Engine::Math::Vector3& center, float radius, const Engine::Math::Vector4& color);
    void DrawBox(const Engine::Math::Vector3& min, const Engine::Math::Vector3& max, const Engine::Math::Vector4& color);
    
    void Render(const RenderContext& context);
    void Clear();

private:
    DebugRenderer() = default;
    ~DebugRenderer() = default;

    static DebugRenderer* instance_;

    struct Vertex {
        Engine::Math::Vector3 position;
        Engine::Math::Vector4 color;
    };

    std::vector<Vertex> vertices_;
    std::unique_ptr<StructuredBuffer> vertexBuffer_;
};

} // namespace Engine::Graphics
