#include "RenderTexture.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Common/Assert.h"
#include <d3dx12.h>

namespace Engine::Graphics {

RenderTexture::RenderTexture() = default;
RenderTexture::~RenderTexture() { Release(); }

void RenderTexture::Create(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, 
                           const Math::Vector2Int& size, DXGI_FORMAT format, const Math::Vector4& clearColor) {
    clearColor_ = clearColor;
    
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = size.x;
    desc.Height = size.y;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format = format;
    optimizedClearValue.Color[0] = clearColor.x;
    optimizedClearValue.Color[1] = clearColor.y;
    optimizedClearValue.Color[2] = clearColor.z;
    optimizedClearValue.Color[3] = clearColor.w;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr = device->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        &optimizedClearValue,
        IID_PPV_ARGS(&resource_)
    );
    Assert(SUCCEEDED(hr), "Failed to create RenderTexture resource.");

    // RTVの作成
    rtvHandle_ = rtvHeap->Allocate();
    device->GetDevice()->CreateRenderTargetView(resource_.Get(), nullptr, rtvHandle_);

    // SRVの作成
    srvIndex_ = srvHeap->AllocateIndex();
    srvHandle_ = srvHeap->GetGPUHandle(srvIndex_);
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHeap->GetCPUHandle(srvIndex_));
}

void RenderTexture::Release() {
    resource_.Reset();
}

void RenderTexture::Clear(ID3D12GraphicsCommandList* commandList) {
    float color[] = { clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w };
    commandList->ClearRenderTargetView(rtvHandle_, color, 0, nullptr);
}

void RenderTexture::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES stateAfter) {
    if (currentState_ == stateAfter) return;
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_.Get(), currentState_, stateAfter);
    commandList->ResourceBarrier(1, &barrier);
    currentState_ = stateAfter;
}

} // namespace Engine::Graphics
