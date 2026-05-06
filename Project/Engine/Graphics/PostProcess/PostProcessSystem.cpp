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

    // 4レベルのダウンスループバッファを作成
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

    bloomCB_ = std::make_unique<ConstantBuffer>();
    bloomCB_->Create(device, sizeof(BloomParams));
}

void PostProcessSystem::Shutdown() {
    for (uint32_t i = 0; i < kBloomLevels; ++i) {
        downsampleBuffers_[i].reset();
        upsampleBuffers_[i].reset();
    }
    blurCB_.reset();
    bloomCB_.reset();
}

void PostProcessSystem::Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination) {
    auto& sm = ShaderManager::GetInstance();
    auto& graphics = GraphicsEngine::GetInstance();
    auto* srvHeap = graphics.GetSRVHeap();

    // パラメータ更新
    BloomParams bloomParams;
    bloomParams.threshold = 0.5f; 
    bloomParams.intensity = 2.0f; 
    bloomParams.exposure = 1.0f;
    bloomCB_->Update(&bloomParams, sizeof(bloomParams));

    ID3D12DescriptorHeap* heaps[] = { srvHeap->GetHeap() };
    commandList->SetDescriptorHeaps(1, heaps);

    PipelineStateDesc postDesc;
    postDesc.depthEnable = false;
    postDesc.depthWriteEnable = false;
    postDesc.dsvFormat = DXGI_FORMAT_UNKNOWN;

    // --- 1. 輝度抽出 & 最初のダウンサンプリング (1/2) ---
    {
        auto* dst = downsampleBuffers_[0].get();
        dst->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        dst->Clear(commandList);
        
        auto rtv = dst->GetRTVHandle();
        commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)dst->GetSize().x, (float)dst->GetSize().y, 0.0f, 1.0f };
        D3D12_RECT scissor = { 0, 0, dst->GetSize().x, dst->GetSize().y };
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissor);

        postDesc.rtvFormat = dst->GetFormat();
        auto* pso = sm.GetOrCreatePSO("BloomThreshold", postDesc);
        auto* rootSig = sm.GetRootSignature("BloomThreshold");
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), inputSource->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("BloomParams"), bloomCB_->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);
    }

    // --- 2. ダウンサンプリング・チェーン (1/4, 1/8, 1/16) ---
    for (uint32_t i = 1; i < kBloomLevels; ++i) {
        auto* src = downsampleBuffers_[i - 1].get();
        auto* dst = downsampleBuffers_[i].get();

        src->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        dst->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        dst->Clear(commandList);

        auto rtv = dst->GetRTVHandle();
        commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)dst->GetSize().x, (float)dst->GetSize().y, 0.0f, 1.0f };
        D3D12_RECT scissor = { 0, 0, dst->GetSize().x, dst->GetSize().y };
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissor);

        postDesc.rtvFormat = dst->GetFormat();
        auto* pso = sm.GetOrCreatePSO("Blit", postDesc);
        auto* rootSig = sm.GetRootSignature("Blit");
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), src->GetSRVHandle());
        commandList->DrawInstanced(3, 1, 0, 0);
    }

    // --- 3. アップサンプリング & ぼかしチェーン ---
    // 一番下のレベルから順に、ぼかして上のレベルに加算していく
    for (int i = kBloomLevels - 1; i >= 0; --i) {
        auto* target = downsampleBuffers_[i].get();
        auto* work = upsampleBuffers_[i].get();

        // 3a. まず現在のレベルをぼかす (横・縦)
        {
            target->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
            work->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
            work->Clear(commandList);
            auto rtv = work->GetRTVHandle();
            commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

            D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)target->GetSize().x, (float)target->GetSize().y, 0.0f, 1.0f };
            D3D12_RECT scissor = { 0, 0, target->GetSize().x, target->GetSize().y };
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissor);

            postDesc.rtvFormat = work->GetFormat();
            auto* blurPso = sm.GetOrCreatePSO("Blur", postDesc);
            auto* blurRootSig = sm.GetRootSignature("Blur");
            commandList->SetGraphicsRootSignature(blurRootSig->Get());
            commandList->SetPipelineState(blurPso->Get());

            // 横
            BlurParams hParams = { {1.0f, 0.0f}, (float)target->GetSize().x };
            blurCB_->Update(&hParams, sizeof(hParams));
            commandList->SetGraphicsRootDescriptorTable(blurRootSig->GetParameterIndex("gMainTexture"), target->GetSRVHandle());
            commandList->SetGraphicsRootConstantBufferView(blurRootSig->GetParameterIndex("BlurParams"), blurCB_->GetGPUVirtualAddress());
            commandList->DrawInstanced(3, 1, 0, 0);

            // 縦 (work -> target)
            work->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
            target->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
            rtv = target->GetRTVHandle();
            commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

            BlurParams vParams = { {0.0f, 1.0f}, (float)target->GetSize().y };
            blurCB_->Update(&vParams, sizeof(vParams));
            commandList->SetGraphicsRootDescriptorTable(blurRootSig->GetParameterIndex("gMainTexture"), work->GetSRVHandle());
            commandList->SetGraphicsRootConstantBufferView(blurRootSig->GetParameterIndex("BlurParams"), blurCB_->GetGPUVirtualAddress());
            commandList->DrawInstanced(3, 1, 0, 0);
        }

        // 3b. ぼかした結果を、一つ上のレベルに加算する
        if (i > 0) {
            auto* parent = downsampleBuffers_[i - 1].get();
            target->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
            parent->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

            auto rtv = parent->GetRTVHandle();
            commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
            
            D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)parent->GetSize().x, (float)parent->GetSize().y, 0.0f, 1.0f };
            D3D12_RECT scissor = { 0, 0, parent->GetSize().x, parent->GetSize().y };
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissor);

            PipelineStateDesc additiveDesc;
            additiveDesc.depthEnable = false;
            additiveDesc.dsvFormat = DXGI_FORMAT_UNKNOWN;
            additiveDesc.rtvFormat = parent->GetFormat();
            additiveDesc.blendEnable = true;
            additiveDesc.blendDesc.RenderTarget[0].BlendEnable = TRUE;
            additiveDesc.blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
            additiveDesc.blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            additiveDesc.blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            
            auto* blitPso = sm.GetOrCreatePSO("Blit", additiveDesc);
            auto* blitRootSig = sm.GetRootSignature("Blit");
            commandList->SetGraphicsRootSignature(blitRootSig->Get());
            commandList->SetPipelineState(blitPso->Get());
            commandList->SetGraphicsRootDescriptorTable(blitRootSig->GetParameterIndex("gMainTexture"), target->GetSRVHandle());
            commandList->DrawInstanced(3, 1, 0, 0);
        }
    }

    // --- 4. 最終合成 & トーンマッピング ---
    {
        downsampleBuffers_[0]->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        inputSource->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

        commandList->OMSetRenderTargets(1, &outputDestination, FALSE, nullptr);
        
        D3D12_VIEWPORT fullViewport = { 0.0f, 0.0f, (float)size_.x, (float)size_.y, 0.0f, 1.0f };
        D3D12_RECT fullScissor = { 0, 0, size_.x, size_.y };
        commandList->RSSetViewports(1, &fullViewport);
        commandList->RSSetScissorRects(1, &fullScissor);

        PipelineStateDesc finalPassDesc;
        finalPassDesc.depthEnable = false;
        finalPassDesc.dsvFormat = DXGI_FORMAT_UNKNOWN;
        finalPassDesc.rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        auto* pso = sm.GetOrCreatePSO("PostProcess", finalPassDesc);
        auto* rootSig = sm.GetRootSignature("PostProcess");
        
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), inputSource->GetSRVHandle());
        commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gBloomTexture"), downsampleBuffers_[0]->GetSRVHandle());
        commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("BloomParams"), bloomCB_->GetGPUVirtualAddress());

        commandList->DrawInstanced(3, 1, 0, 0);
    }
}

} // namespace Engine::Graphics
