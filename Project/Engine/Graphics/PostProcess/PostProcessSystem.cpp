#include "PostProcessSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include <d3dx12.h>

namespace Engine::Graphics {

void PostProcessSystem::Initialize(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, const Engine::Math::Vector2Int& size) {
    device_ = device;
    size_ = size;

    // 1/2 サイズのバッファを作成
    Engine::Math::Vector2Int halfSize = { size.x / 2, size.y / 2 };
    
    brightBuffer_ = std::make_unique<RenderTexture>();
    brightBuffer_->Create(device, rtvHeap, srvHeap, halfSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0,0,0,0});
    
    blurBuffer_ = std::make_unique<RenderTexture>();
    blurBuffer_->Create(device, rtvHeap, srvHeap, halfSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0,0,0,0});

    blurCB_ = std::make_unique<ConstantBuffer>();
    blurCB_->Create(device, sizeof(BlurParams));
}

void PostProcessSystem::Shutdown() {
    brightBuffer_.reset();
    blurBuffer_.reset();
    blurCB_.reset();
}

void PostProcessSystem::Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination) {
    auto& sm = ShaderManager::GetInstance();
    auto& graphics = GraphicsEngine::GetInstance();
    auto* srvHeap = graphics.GetSRVHeap();

    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)size_.x / 2, (float)size_.y / 2, 0.0f, 1.0f };
    D3D12_RECT scissor = { 0, 0, size_.x / 2, size_.y / 2 };

    // --- 1. 輝度抽出 ---
    {
        brightBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        brightBuffer_->Clear(commandList);
        auto rtv = brightBuffer_->GetRTVHandle();
        commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissor);

        auto* pso = sm.GetOrCreatePSO("BloomThreshold", PipelineStateDesc());
        auto* rootSig = sm.GetRootSignature("BloomThreshold");
        if (!pso || !rootSig) return;

        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        ID3D12DescriptorHeap* heaps[] = { srvHeap->GetHeap() };
        commandList->SetDescriptorHeaps(1, heaps);

        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), inputSource->GetSRVHandle());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);
    }

    // --- 2. ぼかし (横) ---
    {
        brightBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        blurBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        blurBuffer_->Clear(commandList);
        auto rtv = blurBuffer_->GetRTVHandle();
        commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        auto* pso = sm.GetOrCreatePSO("Blur", PipelineStateDesc());
        auto* rootSig = sm.GetRootSignature("Blur");
        if (!pso || !rootSig) return;

        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        BlurParams params = { {1.0f, 0.0f}, (float)size_.x / 2 };
        blurCB_->Update(&params, sizeof(params));

        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), brightBuffer_->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("BlurParams"), blurCB_->GetGPUVirtualAddress());
        commandList->DrawInstanced(3, 1, 0, 0);
    }

    // --- 3. ぼかし (縦) ---
    {
        blurBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        brightBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        brightBuffer_->Clear(commandList);
        auto rtv = brightBuffer_->GetRTVHandle();
        commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        BlurParams params = { {0.0f, 1.0f}, (float)size_.y / 2 };
        blurCB_->Update(&params, sizeof(params));

        auto* rootSig = sm.GetRootSignature("Blur");
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), blurBuffer_->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("BlurParams"), blurCB_->GetGPUVirtualAddress());
        commandList->DrawInstanced(3, 1, 0, 0);
    }

    // --- 4. 最終合成 & トーンマッピング ---
    {
        brightBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        
        float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        commandList->ClearRenderTargetView(outputDestination, clearColor, 0, nullptr);
        commandList->OMSetRenderTargets(1, &outputDestination, FALSE, nullptr);
        
        D3D12_VIEWPORT fullViewport = { 0.0f, 0.0f, (float)size_.x, (float)size_.y, 0.0f, 1.0f };
        D3D12_RECT fullScissor = { 0, 0, size_.x, size_.y };
        commandList->RSSetViewports(1, &fullViewport);
        commandList->RSSetScissorRects(1, &fullScissor);

        PipelineStateDesc finalPassDesc;
        finalPassDesc.rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        auto* pso = sm.GetOrCreatePSO("PostProcess", finalPassDesc);
        auto* rootSig = sm.GetRootSignature("PostProcess");
        if (!pso || !rootSig) return;

        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), inputSource->GetSRVHandle());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gBloomTexture"), brightBuffer_->GetSRVHandle());
        commandList->DrawInstanced(3, 1, 0, 0);
    }
}

} // namespace Engine::Graphics
