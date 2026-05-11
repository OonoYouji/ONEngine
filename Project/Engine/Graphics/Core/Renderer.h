#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>
#include "Engine/Graphics/Core/RenderContext.h"
#include "Engine/Graphics/Shader/PipelineState.h"

namespace Engine::Graphics {

class RenderDevice;
class StructuredBuffer;

///
/// 1つの描画要求を表す構造体
///
struct RenderRequest {
    uint32_t modelIndex;
    uint32_t materialIndex;
    uint32_t subMeshIndex;
    Engine::Math::Matrix4x4 world;
    uint32_t entityID;
    uint32_t postProcessFlags;
    uint32_t vertexOffset;
    bool isSkinned = false;
    uint32_t skeletonIndex = 0;
};

///
/// 描画パイプライン全体を管理するクラス
///
class Renderer {
public:
    static constexpr uint32_t kMaxInstances = 10000;
    static constexpr uint32_t kBufferCount = 3;

    static Renderer& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new Renderer();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    void PushRequest(const RenderRequest& request);
    void ClearQueue();
    void Extract();

    void RenderZPrepass(const RenderContext& context);
    void Render(const RenderContext& context);

private:
    Renderer() = default;
    ~Renderer() = default;

    static Renderer* instance_;

    void RenderInternal(const RenderContext& context, const PipelineStateDesc& baseDesc);

    RenderDevice* device_ = nullptr;
    std::vector<RenderRequest> queue_;
    std::unique_ptr<StructuredBuffer> instanceSBs_[kBufferCount];
};

} // namespace Engine::Graphics
