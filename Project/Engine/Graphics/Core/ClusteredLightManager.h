#pragma once

#include <memory>
#include <vector>
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Graphics/Shader/PipelineState.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

///
/// クラスタライトカリングを管理するクラス
///
class ClusteredLightManager {
public:
    static constexpr uint32_t kGridX = 16;
    static constexpr uint32_t kGridY = 9;
    static constexpr uint32_t kGridZ = 24;
    static constexpr uint32_t kNumClusters = kGridX * kGridY * kGridZ;
    static constexpr uint32_t kMaxLightsPerCluster = 64;

    ClusteredLightManager();
    ~ClusteredLightManager();

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief クラスタの AABB を構築 (プロジェクション行列変更時に呼び出す)
    void BuildClusters(ID3D12GraphicsCommandList* commandList, const Engine::Math::Matrix4x4& invProj, const Engine::Math::Vector2Int& screenSize, float nearZ, float farZ);

    /// @brief ライトをクラスタに割り当て (毎フレーム)
    void AssignLights(ID3D12GraphicsCommandList* commandList, D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress, D3D12_GPU_VIRTUAL_ADDRESS pointLightBufferAddress, uint32_t totalLights);

    // バッファ取得
    StructuredBuffer* GetLightGridBuffer() const { return lightGridBuffer_.get(); }
    StructuredBuffer* GetLightIndexListBuffer() const { return lightIndexListBuffer_.get(); }

private:
    RenderDevice* device_ = nullptr;

    std::unique_ptr<StructuredBuffer> clusterAABBBuffer_;
    std::unique_ptr<StructuredBuffer> lightGridBuffer_;
    std::unique_ptr<StructuredBuffer> lightIndexListBuffer_;
    std::unique_ptr<StructuredBuffer> globalIndexCountBuffer_;
    std::unique_ptr<ConstantBuffer> clusterParamsCB_;

    struct ClusterParams {
        Engine::Math::Matrix4x4 invProj;
        float nearZ;
        float farZ;
        float screenWidth;
        float screenHeight;
        uint32_t totalLights;
        float padding[3];
    };

    bool clustersBuilt_ = false;
};

} // namespace Engine::Graphics
