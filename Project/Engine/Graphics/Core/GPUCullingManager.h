#pragma once

#include <memory>
#include <vector>
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// GPU駆動のカリングを管理するクラス
///
class GPUCullingManager {
public:
    GPUCullingManager();
    ~GPUCullingManager();

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief カリングの実行
    void Execute(ID3D12GraphicsCommandList* commandList, 
                 D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress,
                 StructuredBuffer* inputInstances,
                 uint32_t maxInstances,
                 const Engine::Math::Matrix4x4& viewProj,
                 D3D12_GPU_VIRTUAL_ADDRESS meshInfoBufferAddress,
                 uint32_t targetModelIndex,
                 uint32_t instanceOffset,
                 uint32_t batchIndex);

    /// @brief カウンタのリセット (全フレームの最初)
    void ResetCounters(ID3D12GraphicsCommandList* commandList);

    /// @brief バッチインデックスのリセット (Passの最初)
    void ResetBatchIndex() { currentBatchCBIndex_ = 0; }

    StructuredBuffer* GetOutputInstances() const { return outputInstances_.get(); }
    StructuredBuffer* GetDrawArgsBuffer() const { return drawArgsBuffer_.get(); }
    StructuredBuffer* GetIndirectCommandBuffer() const { return indirectCommandBuffer_.get(); }
    ID3D12CommandSignature* GetCommandSignature() const { return commandSignature_.Get(); }

private:
    void CreateCommandSignature();

private:
    RenderDevice* device_ = nullptr;

    static constexpr uint32_t kMaxBatches = 64;

    std::unique_ptr<StructuredBuffer> outputInstances_;
    std::unique_ptr<StructuredBuffer> drawArgsBuffer_; // [0] = 可視インスタンス数
    std::unique_ptr<StructuredBuffer> indirectCommandBuffer_;
    std::unique_ptr<ConstantBuffer> frustumCB_;
    std::unique_ptr<ConstantBuffer> cullingParamsCBs_[kMaxBatches];
    uint32_t currentBatchCBIndex_ = 0;

    ComPtr<ID3D12CommandSignature> commandSignature_;

    struct FrustumPlanes {
        Engine::Math::Vector4 planes[6];
    };

    struct CullingParams {
        uint32_t targetModelIndex;
        uint32_t maxInstances;
        uint32_t instanceOffset;
        uint32_t batchIndex;
    };
};

} // namespace Engine::Graphics
