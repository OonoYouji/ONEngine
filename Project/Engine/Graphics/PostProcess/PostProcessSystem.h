#pragma once

#include <d3d12.h>
#include <memory>
#include <string>
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

///
/// ポストプロセスを一括管理するシステム
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
    /// @param commandList コマンドリスト
    /// @param inputSource 入力となるHDRテクスチャ
    /// @param outputDestination 出力先（SwapChainのバックバッファRTVハンドルなど）
    void Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination);

private:
    RenderDevice* device_ = nullptr;
    
    // 中間バッファ（ブルーム等で使用）
    // std::unique_ptr<RenderTexture> bloomBuffer_;
};

} // namespace Engine::Graphics
