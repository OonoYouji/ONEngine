#pragma once

#include "Engine/ECS/Registry.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <vector>

namespace Engine::Graphics {
    class RenderDevice;
}

namespace Engine::ECS {

///
/// ボーン更新とスキニング計算を行うシステム (Additive)
///
class AnimationSystem {
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief ボーン行列の更新とスキニングの実行
    void Update(Registry& registry, ID3D12GraphicsCommandList* commandList, float dt, uint32_t frameIndex);

    D3D12_GPU_VIRTUAL_ADDRESS GetBoneBufferAddress(uint32_t frameIndex) const { return boneMatrixSBs_[frameIndex]->GetResource()->GetGPUVirtualAddress(); }
    Graphics::StructuredBuffer* GetSkinnedVertexBuffer() { return skinnedVertexBuffer_.get(); }

    private:
    Graphics::RenderDevice* device_ = nullptr;

    // トリプルバッファ化
    static constexpr uint32_t kBufferCount = 3;
    std::unique_ptr<Graphics::StructuredBuffer> boneMatrixSBs_[kBufferCount];

    std::unique_ptr<Graphics::StructuredBuffer> skinnedVertexBuffer_;

    static constexpr uint32_t kMaxBatches = 64;
    std::unique_ptr<Graphics::ConstantBuffer> skinningParamsCBs_[kMaxBatches];

    static constexpr uint32_t kMaxBones = 1024 * 64; 
    static constexpr uint32_t kMaxSkinnedVertices = 2000000; 

    float accumulatedTime_ = 0.0f;
    };

} // namespace Engine::ECS
