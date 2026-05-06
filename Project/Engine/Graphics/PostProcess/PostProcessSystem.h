#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

///
/// ポストプロセスを一括管理するシステム (Bloom対応)
///
class PostProcessSystem {
public:
    static PostProcessSystem& GetInstance() {
        static PostProcessSystem instance;
        return instance;
    }

    void Initialize(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, const Engine::Math::Vector2Int& size);
    void Shutdown();

    /// @brief ポストプロセスの実行
    void Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination);

private:
    RenderDevice* device_ = nullptr;
    Engine::Math::Vector2Int size_;
    
    // ブルーム用中間バッファ (4レベル)
    static constexpr uint32_t kBloomLevels = 4;
    std::unique_ptr<RenderTexture> downsampleBuffers_[kBloomLevels];
    std::unique_ptr<RenderTexture> upsampleBuffers_[kBloomLevels];
    
    struct BlurParams {
        Engine::Math::Vector2 direction;
        float textureSize;
        float padding;
    };
    std::unique_ptr<ConstantBuffer> blurCB_;

    struct BloomParams {
        float threshold;
        float intensity;
        float exposure;
        float padding;
    };
    std::unique_ptr<ConstantBuffer> bloomCB_;
};

} // namespace Engine::Graphics
