#include "PostProcessSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Common/Console.h"
#include <d3dx12.h>

namespace Engine::Graphics {

PostProcessSystem* PostProcessSystem::instance_ = nullptr;

void PostProcessSystem::Initialize(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, const Engine::Math::Vector2Int& size) {
    device_ = device;
    size_ = size;

    Engine::Math::Vector2Int currentSize = size;
    for (uint32_t i = 0; i < kBloomLevels; ++i) {
        currentSize.x /= 2;
        currentSize.y /= 2;
        if (currentSize.x < 1) currentSize.x = 1;
        if (currentSize.y < 1) currentSize.y = 1;

        downsampleBuffers_[i] = std::make_unique<RenderTexture>();
        downsampleBuffers_[i]->Create(device, rtvHeap, srvHeap, currentSize, DXGI_FORMAT_R16G16B16A16_FLOAT, { 0,0,0,0 });
        downsampleBuffers_[i]->SetDebugName(std::format("BloomDownsample_{}", i));

        upsampleBuffers_[i] = std::make_unique<RenderTexture>();
        upsampleBuffers_[i]->Create(device, rtvHeap, srvHeap, currentSize, DXGI_FORMAT_R16G16B16A16_FLOAT, { 0,0,0,0 });
        upsampleBuffers_[i]->SetDebugName(std::format("BloomUpsample_{}", i));
    }

    blurCB_ = std::make_unique<ConstantBuffer>();
    blurCB_->Create(device, sizeof(BlurParams));

    postProcessCB_ = std::make_unique<ConstantBuffer>();
    postProcessCB_->Create(device, sizeof(PostProcessParams));
}

void PostProcessSystem::Shutdown() {
    for (uint32_t i = 0; i < kBloomLevels; ++i) {
        downsampleBuffers_[i].reset();
        upsampleBuffers_[i].reset();
    }
    blurCB_.reset();
    postProcessCB_.reset();
}

void PostProcessSystem::Render(ID3D12GraphicsCommandList* commandList, 
                               RenderTexture* mainTexture, 
                               RenderTexture* bloomTexture,
                               RenderTexture* normalTexture,
                               RenderTexture* idTexture,
                               D3D12_CPU_DESCRIPTOR_HANDLE outputDestination) {
    auto& sm = ShaderManager::GetInstance();
    
    // --- 1. Threshold & Bloom ---
    // (現在は簡略化のため、ダミーとして黒いテクスチャを使用)
    RenderTexture* sourceBloom = bloomTexture ? bloomTexture : downsampleBuffers_[0].get();
    if (!bloomTexture) {
        sourceBloom->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        sourceBloom->Clear(commandList);
        sourceBloom->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    }

    // --- 2. Final Post Process ---
    PostProcessParams params;
    params.threshold = 1.0f;
    params.intensity = 1.0f;
    params.exposure = 1.0f;
    params.outlineColor = { 0, 0, 0, 1 };
    params.outlineThreshold = 0.5f;
    params.outlineWidth = 1.0f;
    postProcessCB_->Update(&params, sizeof(params));

    PipelineStateDesc desc;
    desc.numRenderTargets = 1;
    desc.rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.depthEnable = false;
    desc.depthWriteEnable = false;

    auto* pso = sm.GetOrCreatePSO("PostProcess", desc);
    auto* rootSig = sm.GetRootSignature("PostProcess");
    if (pso && rootSig) {
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        commandList->OMSetRenderTargets(1, &outputDestination, FALSE, nullptr);
        
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)size_.x, (float)size_.y, 0.0f, 1.0f };
        D3D12_RECT scissor = { 0, 0, size_.x, size_.y };
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissor);

        // 各テクスチャをバインド
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), mainTexture->GetSRVHandle());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gBloomTexture"), sourceBloom->GetSRVHandle());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gNormalTexture"), normalTexture->GetSRVHandle());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gIDTexture"), idTexture->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("PostProcessParams"), postProcessCB_->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);
    }
}

} // namespace Engine::Graphics
