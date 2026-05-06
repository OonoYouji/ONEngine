#pragma once

#include <vector>
#include <memory>
#include <d3d12.h>
#include "Engine/Core/Math/Math.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Shader/PipelineState.h"

namespace Engine::Graphics {

class RenderDevice;
struct RenderContext;

///
/// デバッグ用の線やワイヤーフレームを描画するクラス
///
class DebugRenderer {
public:
    struct Vertex {
        Engine::Math::Vector3 position;
        Engine::Math::Vector4 color;
    };

    static DebugRenderer& GetInstance() {
        static DebugRenderer instance;
        return instance;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief 線の描画
    void DrawLine(const Engine::Math::Vector3& start, const Engine::Math::Vector3& end, const Engine::Math::Vector4& color = {1, 1, 1, 1});

    /// @brief ワイヤーフレームボックスの描画
    void DrawBox(const Engine::Math::Vector3& center, const Engine::Math::Vector3& size, const Engine::Math::Vector4& color = {1, 1, 1, 1});

    /// @brief 描画実行
    void Render(const RenderContext& context);

    /// @brief キューのクリア（毎フレーム呼ぶ）
    void Clear();

private:
    DebugRenderer() = default;
    ~DebugRenderer() = default;

private:
    RenderDevice* device_ = nullptr;
    std::vector<Vertex> vertices_;
    
    std::unique_ptr<StructuredBuffer> vertexBuffer_;
    static constexpr uint32_t kMaxVertices = 100000;
};

} // namespace Engine::Graphics
