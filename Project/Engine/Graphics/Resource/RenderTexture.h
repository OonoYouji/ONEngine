#pragma once

#include <string>
#include <memory>
#include <d3d12.h>
#include "D3D12MemAlloc.h"
#include "Engine/Graphics/Utils/ComPtr.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

///
/// オフスクリーン描画用のテクスチャリソース
///
class RenderTexture {
public:
    RenderTexture();
    ~RenderTexture();

    /// @brief 初期化
    /// @param device デバイス
    /// @param rtvHeap RTV用ヒープ
    /// @param srvHeap SRV用ヒープ
    /// @param size サイズ
    /// @param format フォーマット
    void Create(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, 
                const Math::Vector2Int& size, DXGI_FORMAT format, const Math::Vector4& clearColor = {0,0,0,1});

    /// @brief リソースの破棄
    void Release();

    /// @brief 描画ターゲットとしてのクリア
    void Clear(ID3D12GraphicsCommandList* commandList);

    /// @brief 状態遷移（描画用 <-> シェーダー参照用）
    void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES stateAfter);

    void SetDebugName(const std::string& name);
    const std::string& GetDebugName() const { return debugName_; }

    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return rtvHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const { return srvHandle_; }
    uint32_t GetSRVIndex() const { return srvIndex_; }
    const Math::Vector2Int& GetSize() const { return size_; }
    DXGI_FORMAT GetFormat() const { return format_; }

private:
    ComPtr<D3D12MA::Allocation> allocation_;
    Math::Vector2Int size_;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_ = {};
    uint32_t srvIndex_ = 0xFFFFFFFF;
    Math::Vector4 clearColor_;
    D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_COMMON;
    std::string debugName_;

    void optimizedClearColor(float* dest, const Math::Vector4& src);
};

} // namespace Engine::Graphics
