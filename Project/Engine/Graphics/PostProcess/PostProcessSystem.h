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
    
    // ブルーム用中間バッファ (1/2サイズ)
    std::unique_ptr<RenderTexture> brightBuffer_;
    std::unique_ptr<RenderTexture> blurBuffer_; // ぼかし用ワークバッファ
    
    struct BlurParams {
        Engine::Math::Vector2 direction;
        float textureSize;
        float padding;
    };
    std::unique_ptr<ConstantBuffer> blurCB_;
};

} // namespace Engine::Graphics
