#include "ParticleSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Core/Timer.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/AssetManager.h"
#include <d3dx12.h>

namespace Engine::ECS {

void ParticleSystem::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    meshInfoBuffer_ = std::make_unique<Graphics::StructuredBuffer>();
    meshInfoBuffer_->Create(device, sizeof(GeneratedSchema::MeshInfo), 1024);
}

void ParticleSystem::Shutdown() {
    emitters_.clear();
    meshInfoBuffer_.reset();
}

void ParticleSystem::UpdateMeshInfoBuffer() {
    auto& assetManager = Asset::AssetManager::GetInstance();
    std::vector<GeneratedSchema::MeshInfo> infos(1024);
    
    for (uint32_t i = 1; i < 1024; ++i) {
        const auto& meshes = assetManager.GetMeshesByIndex(i);
        if (!meshes.empty()) {
            infos[i].vertexOffset = meshes[0]->GetVertexOffset();
            infos[i].indexOffset = meshes[0]->GetIndexOffset();
            infos[i].vertexCount = meshes[0]->GetVertexCount();
            infos[i].indexCount = meshes[0]->GetIndexCount();
        }
    }
    meshInfoBuffer_->Update(infos.data(), (uint32_t)(infos.size() * sizeof(GeneratedSchema::MeshInfo)));
}

void ParticleSystem::Update(Registry& registry) {
    auto& sm = ::Engine::Graphics::ShaderManager::GetInstance();
    auto& graphics = ::Engine::Graphics::GraphicsEngine::GetInstance();
    auto* commandList = graphics.GetCommandQueue()->GetCommandList();
    
    UpdateMeshInfoBuffer();

    time_ += 0.016f; 
    float dt = 0.016f; 

    registry.GetView<Transform, ParticleEmitter>().Each([&](Entity entity, Transform& transform, ParticleEmitter& emitter) {
        auto& state = emitters_[entity];

        if (!state.initialized) {
            state.particleBuffer = std::make_unique<Graphics::StructuredBuffer>();
            state.particleBuffer->Create(device_, sizeof(GeneratedSchema::ParticleGPUData), emitter.count, nullptr, true); // isUAV = true
            
            state.updateCB = std::make_unique<Graphics::ConstantBuffer>();
            state.updateCB->Create(device_, sizeof(UpdateParams));

            std::vector<GeneratedSchema::ParticleGPUData> initial(emitter.count);
            for(uint32_t i=0; i<emitter.count; ++i) {
                initial[i].age = emitter.lifetime * ((float)i / (float)emitter.count);
                initial[i].maxLifetime = emitter.lifetime;
                initial[i].color = emitter.startColor;
                initial[i].scale = emitter.startScale;
                initial[i].modelIndex = emitter.modelIndex;
                initial[i].textureIndex = emitter.textureIndex;
                initial[i].entityID = static_cast<uint32_t>(entity);
                initial[i].postProcessFlags = 0;
            }
            
            // DEFAULTヒープへの初期化（ステージングバッファ経由）
            auto stagingBuffer = std::make_unique<Graphics::StructuredBuffer>();
            stagingBuffer->Create(device_, sizeof(GeneratedSchema::ParticleGPUData), emitter.count, initial.data(), false); // isUAV = false (UPLOAD)

            state.particleBuffer->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
            commandList->CopyBufferRegion(state.particleBuffer->GetResource(), 0, stagingBuffer->GetResource(), 0, emitter.count * sizeof(GeneratedSchema::ParticleGPUData));
            state.particleBuffer->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

            // ステージングバッファの遅延削除を考慮する必要があるが、ここでは簡易的に保持し、後で破棄されることを期待。
            // 実際にはコマンドリストの完了を待ってから破棄すべきだが、現在のエンジンの仕組みに合わせて実装。
            state.stagingBuffers.push_back(std::move(stagingBuffer));

            state.count = emitter.count;
            state.initialized = true;
        }

        auto* pso = sm.GetComputePSO("ParticleUpdate");
        auto* rootSig = sm.GetRootSignature("ParticleUpdate");

        if (pso && rootSig) {
            state.particleBuffer->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            commandList->SetComputeRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso);

            ParticleSystem::UpdateParams params;
            params.dt = dt;
            params.emitterPos = transform.position;
            params.totalParticles = emitter.count;

            params.seed = time_;
            params.speed = emitter.speed;
            params.speedRandom = emitter.speedRandom;
            params.lifetime = emitter.lifetime;

            params.lifetimeRandom = emitter.lifetimeRandom;
            params.spreadAngle = emitter.spreadAngle;
            params.gravity = emitter.gravity;
            params.startScale = emitter.startScale;

            params.endScale = emitter.endScale;
            params.modelIndex = emitter.modelIndex;
            params.textureIndex = emitter.textureIndex;
            params.padding = 0.0f;

            params.startColor = emitter.startColor;
            params.endColor = emitter.endColor;

            state.updateCB->Update(&params, sizeof(params));

            auto paramsIdx = rootSig->GetParameterIndex("gParams");
            if (paramsIdx != ::Engine::Graphics::RootSignature::kInvalidIndex)
                commandList->SetComputeRootConstantBufferView(paramsIdx, state.updateCB->GetGPUVirtualAddress());

            auto uavIdx = rootSig->GetParameterIndex("gParticles");
            if (uavIdx != ::Engine::Graphics::RootSignature::kInvalidIndex)
                commandList->SetComputeRootUnorderedAccessView(uavIdx, state.particleBuffer->GetResource()->GetGPUVirtualAddress());

            commandList->Dispatch((emitter.count + 255) / 256, 1, 1);
            state.particleBuffer->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        }
    });
}

void ParticleSystem::Render(Registry& registry, const Graphics::RenderContext& context) {
    auto& sm = ::Engine::Graphics::ShaderManager::GetInstance();
    auto& geoPool = ::Engine::Graphics::GeometryPool::GetInstance();
    auto& tm = ::Engine::Asset::TextureManager::GetInstance();

    Graphics::PipelineStateDesc desc;
    desc.numRenderTargets = context.numRenderTargets;
    for (uint32_t i = 0; i < context.numRenderTargets; ++i) {
        desc.rtvFormats[i] = context.rtvFormats[i];
    }

    // Mesh Shader を使用するため ID3D12GraphicsCommandList6 にキャスト
    auto* pCmd6 = static_cast<ID3D12GraphicsCommandList6*>(context.commandList);

    registry.GetView<Transform, ParticleEmitter>().Each([pCmd6, &sm, &geoPool, &tm, &desc, &context, this](Entity entity, Transform& transform, ParticleEmitter& emitter) {
        auto it = emitters_.find(entity);
        if (it == emitters_.end() || !it->second.initialized) return;
        auto& state = it->second;

        auto* pso = sm.GetOrCreateMeshPSO("ParticleRender", desc);
        auto* rootSig = sm.GetRootSignature("ParticleRender");

        if (pso && rootSig && pCmd6) {
            pCmd6->SetGraphicsRootSignature(rootSig->Get());
            pCmd6->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { tm.GetSrvHeap()->GetHeap() };
            pCmd6->SetDescriptorHeaps(1, heaps);

            auto setSRV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
                auto idx = rootSig->GetParameterIndex(name);
                if (idx != ::Engine::Graphics::RootSignature::kInvalidIndex)
                    pCmd6->SetGraphicsRootShaderResourceView(idx, addr);
            };

            auto setCBV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
                auto idx = rootSig->GetParameterIndex(name);
                if (idx != ::Engine::Graphics::RootSignature::kInvalidIndex)
                    pCmd6->SetGraphicsRootConstantBufferView(idx, addr);
            };

            setCBV("gSceneData", context.sceneCBAddress);
            setSRV("gParticles", state.particleBuffer->GetResource()->GetGPUVirtualAddress());
            setSRV("gVertices", geoPool.GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            setSRV("gIndices", geoPool.GetIndexBuffer()->GetResource()->GetGPUVirtualAddress());
            setSRV("gMeshInfos", meshInfoBuffer_->GetResource()->GetGPUVirtualAddress());
            
            auto texIdx = rootSig->GetParameterIndex("gTextures");
            if (texIdx != ::Engine::Graphics::RootSignature::kInvalidIndex)
                pCmd6->SetGraphicsRootDescriptorTable(texIdx, tm.GetSrvHeap()->GetGPUHandle(0));

            pCmd6->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            pCmd6->DispatchMesh((emitter.count + 127) / 128, 1, 1);
        }
    });
}

} // namespace Engine::ECS
