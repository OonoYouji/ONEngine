#include "PipelineState.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Graphics {

// Pipeline State Stream のための構造体定義
struct PipelineStateStream {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_AS AS;
    CD3DX12_PIPELINE_STATE_STREAM_MS MS;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC Blend;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
};

PipelineState::PipelineState() = default;
PipelineState::~PipelineState() = default;

bool PipelineState::Create(
    RenderDevice* device,
    RootSignature* rootSignature,
    ShaderObject* vs,
    ShaderObject* ps,
    ShaderObject* as,
    ShaderObject* ms,
    const PipelineStateDesc& desc
) {
    PipelineStateStream stream;

    // 1. ルートシグネチャ
    stream.RootSignature = rootSignature->Get();

    // 2. シェーダー
    if (vs && vs->blob) stream.VS = { vs->blob->GetBufferPointer(), vs->blob->GetBufferSize() };
    if (ps && ps->blob) stream.PS = { ps->blob->GetBufferPointer(), ps->blob->GetBufferSize() };
    if (as && as->blob) stream.AS = { as->blob->GetBufferPointer(), as->blob->GetBufferSize() };
    if (ms && ms->blob) stream.MS = { ms->blob->GetBufferPointer(), ms->blob->GetBufferSize() };

    // 3. ラスタライザ
    CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
    rasterizerDesc.FillMode = desc.fillMode;
    rasterizerDesc.CullMode = desc.cullMode;
    stream.Rasterizer = rasterizerDesc;

    // 4. ブレンド
    CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
    if (desc.blendEnable) {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    }
    else {
        blendDesc.RenderTarget[0] = desc.blendDesc.RenderTarget[0];
    }
    stream.Blend = blendDesc;

    // 5. 深度ステンシル
    CD3DX12_DEPTH_STENCIL_DESC depthDesc(D3D12_DEFAULT);
    depthDesc.DepthEnable = desc.depthEnable;
    depthDesc.DepthWriteMask = desc.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depthDesc.DepthFunc = desc.depthFunc;
    stream.DepthStencil = depthDesc;

    // 6. 入力レイアウト (Manual Vertex Fetching を前提とするため空)
    D3D12_INPUT_LAYOUT_DESC inputLayout = { nullptr, 0 };
    stream.InputLayout = inputLayout;

    // 7. トポロジーとフォーマット
    stream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 1;
    rtvFormats.RTFormats[0] = desc.rtvFormat;
    stream.RTVFormats = rtvFormats;
    
    stream.DSVFormat = desc.dsvFormat;

    // 8. PSOの作成 (ID3D12Device2 以降が必要)
    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = { sizeof(stream), &stream };
    
    ComPtr<ID3D12Device2> device2;
    HRESULT hr = device->GetDevice()->QueryInterface(IID_PPV_ARGS(&device2));
    if (FAILED(hr)) {
        Console::Log("Failed to query ID3D12Device2. Mesh Shaders may not be supported.");
        return false;
    }

    hr = device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(hr)) {
        Engine::Console::LogError(std::format("Failed to create pipeline state using Stream. Error: {}", Engine::HrToString(hr)));
        return false;
    }

    return true;
}

} // namespace Engine::Graphics
