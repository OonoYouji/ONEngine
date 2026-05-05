#include "PipelineState.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Graphics {

// Graphics Pipeline 用のストリーム構造体
struct GraphicsPipelineStateStream {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC Blend;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
};

// Mesh Shader Pipeline 用のストリーム構造体
struct MeshPipelineStateStream {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
    CD3DX12_PIPELINE_STATE_STREAM_AS AS;
    CD3DX12_PIPELINE_STATE_STREAM_MS MS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC Blend;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
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
    if (vs) Engine::Console::Log(std::format("VS Blob: {}, Size: {}", (void*)vs->blob.Get(), vs->blob ? vs->blob->GetBufferSize() : 0));
    if (ps) Engine::Console::Log(std::format("PS Blob: {}, Size: {}", (void*)ps->blob.Get(), ps->blob ? ps->blob->GetBufferSize() : 0));
    if (as) Engine::Console::Log(std::format("AS Blob: {}, Size: {}", (void*)as->blob.Get(), as->blob ? as->blob->GetBufferSize() : 0));
    if (ms) Engine::Console::Log(std::format("MS Blob: {}, Size: {}", (void*)ms->blob.Get(), ms->blob ? ms->blob->GetBufferSize() : 0));

    HRESULT hr;

    // Mesh Shader が指定されている場合は Stream 方式を使用
    if (ms) {
        ComPtr<ID3D12Device2> device2;
        hr = device->GetDevice()->QueryInterface(IID_PPV_ARGS(&device2));
        if (FAILED(hr)) {
            Console::LogError("Failed to query ID3D12Device2 for Mesh Shaders.");
            return false;
        }

        MeshPipelineStateStream stream;
        stream.RootSignature = rootSignature->Get();
        if (as && as->blob) stream.AS = { as->blob->GetBufferPointer(), as->blob->GetBufferSize() };
        if (ms && ms->blob) stream.MS = { ms->blob->GetBufferPointer(), ms->blob->GetBufferSize() };
        if (desc.usePS && ps && ps->blob) stream.PS = { ps->blob->GetBufferPointer(), ps->blob->GetBufferSize() };

        D3D12_RT_FORMAT_ARRAY rtvFormats = {};
        rtvFormats.NumRenderTargets = desc.numRenderTargets;
        for (uint32_t i = 0; i < desc.numRenderTargets; ++i) {
            rtvFormats.RTFormats[i] = desc.rtvFormat;
        }
        stream.RTVFormats = rtvFormats;
        stream.DSVFormat = desc.dsvFormat;
        stream.PrimitiveTopology = desc.primitiveTopologyType;

        CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
        rasterizerDesc.FillMode = desc.fillMode;
        rasterizerDesc.CullMode = desc.cullMode;
        stream.Rasterizer = rasterizerDesc;

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

        CD3DX12_DEPTH_STENCIL_DESC depthDesc(D3D12_DEFAULT);
        depthDesc.DepthEnable = desc.depthEnable;
        depthDesc.DepthWriteMask = desc.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        depthDesc.DepthFunc = desc.depthFunc;
        stream.DepthStencil = depthDesc;

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = { sizeof(stream), &stream };
        hr = device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState_));
    }
    else {
        // 通常のグラフィックスパイプライン (VS/PS)
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = rootSignature->Get();
        
        if (vs && vs->blob) psoDesc.VS = { vs->blob->GetBufferPointer(), vs->blob->GetBufferSize() };
        if (desc.usePS && ps && ps->blob) psoDesc.PS = { ps->blob->GetBufferPointer(), ps->blob->GetBufferSize() };

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FillMode = desc.fillMode;
        psoDesc.RasterizerState.CullMode = desc.cullMode;

        if (desc.blendEnable) {
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        }
        else {
            psoDesc.BlendState = desc.blendDesc;
        }

        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = desc.depthEnable;
        psoDesc.DepthStencilState.DepthWriteMask = desc.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc = desc.depthFunc;

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = desc.primitiveTopologyType;
        psoDesc.NumRenderTargets = desc.numRenderTargets;
        for (uint32_t i = 0; i < desc.numRenderTargets; ++i) {
            psoDesc.RTVFormats[i] = desc.rtvFormat;
        }
        psoDesc.DSVFormat = desc.dsvFormat;
        psoDesc.SampleDesc.Count = 1;

        hr = device->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
    }

    if (FAILED(hr)) {
        Engine::Console::LogError(std::format("Failed to create pipeline state. Error: {}", Engine::HrToString(hr)));
        return false;
    }

    return true;
}

} // namespace Engine::Graphics
