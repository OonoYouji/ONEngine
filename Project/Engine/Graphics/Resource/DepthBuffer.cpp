#include "DepthBuffer.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Common/Assert.h"

namespace Engine::Graphics {

DepthBuffer::DepthBuffer() = default;
DepthBuffer::~DepthBuffer() = default;

void DepthBuffer::Create(RenderDevice* device, DescriptorHeap* dsvHeap, uint32_t width, uint32_t height) {
    DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 1. リソース作成
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        format,
        width,
        height,
        1, 1, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    HRESULT hr = device->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&resource_)
    );
    Assert(SUCCEEDED(hr), "Failed to create Depth Buffer resource.");

    // 2. DSV作成
    dsvHandle_ = dsvHeap->GetCPUHandle(0); // とりあえず0番目を使用

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    device->GetDevice()->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);
}

} // namespace Engine::Graphics
