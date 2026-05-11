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

        upsampleBuffers_[i] = std::make_unique<RenderTexture>();
        upsampleBuffers_[i]->Create(device, rtvHeap, srvHeap, currentSize, DXGI_FORMAT_R16G16B16A16_FLOAT, { 0,0,0,0 });
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

void PostProcessSystem::Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination) {
    auto& sm = ShaderManager::GetInstance();
    
    // --- 1. Threshold ---
    // (実装省略、本来はここで Bloom のしきい値処理を行う)

    // --- 2. Final Post Process ---
    PostProcessParams params;
    params.threshold = 1.0f;
    params.intensity = 1.0f;
    params.exposure = 1.0f;
    params.outlineColor = { 0, 0, 0, 1 };
    params.outlineThreshold = 0.5f;
    params.outlineWidth = 1.0f;
    postProcessCB_->Update(&params, sizeof(params));

    auto* pso = sm.GetComputePSO("PostProcess");
    auto* rootSig = sm.GetRootSignature("PostProcess");
    if (pso && rootSig) {
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso);

        // outputDestination への描画設定 (本来は OMSetRenderTargets)
        commandList->OMSetRenderTargets(1, &outputDestination, FALSE, nullptr);
        
        // inputSource をテクスチャとしてセット
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gTexture"), inputSource->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("PostProcessParams"), postProcessCB_->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);
    }
}

} // namespace Engine::Graphics
