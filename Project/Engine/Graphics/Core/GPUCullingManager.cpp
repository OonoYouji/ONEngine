#include "GPUCullingManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include <d3dx12.h>
#include "Schema/Schema.h"

namespace Engine::Graphics {

GPUCullingManager::GPUCullingManager() = default;
GPUCullingManager::~GPUCullingManager() = default;

void GPUCullingManager::Initialize(RenderDevice* device) {
    device_ = device;

    outputInstances_ = std::make_unique<StructuredBuffer>();
    outputInstances_->Create(device, sizeof(GeneratedSchema::InstanceData), 2048 * kMaxBatches, nullptr, true); 

    indirectCommandBuffer_ = std::make_unique<StructuredBuffer>();
    indirectCommandBuffer_->Create(device, sizeof(uint32_t) * 5, 2048 * kMaxBatches, nullptr, true); 

    drawArgsBuffer_ = std::make_unique<StructuredBuffer>();
    drawArgsBuffer_->Create(device, sizeof(uint32_t), kMaxBatches, nullptr, true);

    frustumCB_ = std::make_unique<ConstantBuffer>();
    frustumCB_->Create(device, sizeof(FrustumPlanes));

    for (uint32_t i = 0; i < kMaxBatches; ++i) {
        cullingParamsCBs_[i] = std::make_unique<ConstantBuffer>();
        cullingParamsCBs_[i]->Create(device, sizeof(CullingParams));
    }

    CreateCommandSignature();
}

void GPUCullingManager::Shutdown() {
    outputInstances_.reset();
    indirectCommandBuffer_.reset();
    drawArgsBuffer_.reset();
    frustumCB_.reset();
    for (uint32_t i = 0; i < kMaxBatches; ++i) {
        cullingParamsCBs_[i].reset();
    }
    commandSignature_.Reset();
}

void GPUCullingManager::CreateCommandSignature() {
    D3D12_INDIRECT_ARGUMENT_DESC args[1] = {};
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

    D3D12_COMMAND_SIGNATURE_DESC desc = {};
    desc.ByteStride = sizeof(uint32_t) * 5;
    desc.NumArgumentDescs = 1;
    desc.pArgumentDescs = args;

    HRESULT hr = device_->GetDevice()->CreateCommandSignature(&desc, nullptr, IID_PPV_ARGS(&commandSignature_));
    Assert(SUCCEEDED(hr), "Failed to create command signature.");
}

void GPUCullingManager::ResetCounters(ID3D12GraphicsCommandList* commandList) {
    auto& sm = ShaderManager::GetInstance();
    auto* pso = sm.GetComputePSO("CullingReset");
    auto* rootSig = sm.GetRootSignature("CullingReset");

    if (!pso || !rootSig) return;

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandList->ResourceBarrier(1, &barrier);

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gCountBuffer"), drawArgsBuffer_->GetResource()->GetGPUVirtualAddress());

    commandList->Dispatch(1, 1, 1); 

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
    commandList->ResourceBarrier(1, &barrier);

    currentBatchCBIndex_ = 0; 
}

void GPUCullingManager::Execute(ID3D12GraphicsCommandList* commandList, 
                              D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress,
                              StructuredBuffer* inputInstances,
                              uint32_t maxInstances,
                              const Engine::Math::Matrix4x4& vp,
                              D3D12_GPU_VIRTUAL_ADDRESS meshInfoBufferAddress,
                              uint32_t targetModelIndex,
                              uint32_t instanceOffset,
                              uint32_t batchIndex) {
    auto& sm = ShaderManager::GetInstance();
    auto* pso = sm.GetComputePSO("Culling");
    auto* rootSig = sm.GetRootSignature("Culling");

    if (!pso || !rootSig) return;

    FrustumPlanes frustum;
    frustum.planes[0] = { vp.m[0][3] + vp.m[0][0], vp.m[1][3] + vp.m[1][0], vp.m[2][3] + vp.m[2][0], vp.m[3][3] + vp.m[3][0] };
    frustum.planes[1] = { vp.m[0][3] - vp.m[0][0], vp.m[1][3] - vp.m[1][0], vp.m[2][3] - vp.m[2][0], vp.m[3][3] - vp.m[3][0] };
    frustum.planes[2] = { vp.m[0][3] + vp.m[0][1], vp.m[1][3] + vp.m[1][1], vp.m[2][3] + vp.m[2][1], vp.m[3][3] + vp.m[3][1] };
    frustum.planes[3] = { vp.m[0][3] - vp.m[0][1], vp.m[1][3] - vp.m[1][1], vp.m[2][3] - vp.m[2][1], vp.m[3][3] - vp.m[3][1] };
    frustum.planes[4] = { vp.m[0][2], vp.m[1][2], vp.m[2][2], vp.m[3][2] };
    frustum.planes[5] = { vp.m[0][3] - vp.m[0][2], vp.m[1][3] - vp.m[1][2], vp.m[2][3] - vp.m[2][2], vp.m[3][3] - vp.m[3][2] };

    for (int i = 0; i < 6; ++i) {
        float len = sqrtf(frustum.planes[i].x * frustum.planes[i].x + frustum.planes[i].y * frustum.planes[i].y + frustum.planes[i].z * frustum.planes[i].z);
        frustum.planes[i].x /= len;
        frustum.planes[i].y /= len;
        frustum.planes[i].z /= len;
        frustum.planes[i].w /= len;
    }
    frustumCB_->Update(&frustum, sizeof(frustum));

    auto& paramsCB = cullingParamsCBs_[currentBatchCBIndex_];
    struct CullingParams {
        uint32_t targetModelIndex;
        uint32_t maxInstances;
        uint32_t instanceOffset;
        uint32_t batchIndex;
        uint32_t forceVisible;
    };
    CullingParams cParams = { targetModelIndex, maxInstances, instanceOffset, batchIndex, 1 };
    paramsCB->Update(&cParams, sizeof(cParams));

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);

    auto setCBV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
        auto idx = rootSig->GetParameterIndex(name);
        if (idx != RootSignature::kInvalidIndex) commandList->SetComputeRootConstantBufferView(idx, addr);
    };
    auto setSRV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
        auto idx = rootSig->GetParameterIndex(name);
        if (idx != RootSignature::kInvalidIndex) commandList->SetComputeRootShaderResourceView(idx, addr);
    };
    auto setUAV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
        auto idx = rootSig->GetParameterIndex(name);
        if (idx != RootSignature::kInvalidIndex) commandList->SetComputeRootUnorderedAccessView(idx, addr);
    };

    {
        D3D12_RESOURCE_BARRIER preBarriers[3];
        preBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(outputInstances_->GetResource(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        preBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        preBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(indirectCommandBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(3, preBarriers);
    }

    setCBV("gSceneData", sceneCBAddress);
    setCBV("gFrustum", frustumCB_->GetGPUVirtualAddress());
    setCBV("gCullingParams", paramsCB->GetGPUVirtualAddress());
    setSRV("gInputInstances", inputInstances->GetResource()->GetGPUVirtualAddress());
    setSRV("gMeshInfos", meshInfoBufferAddress);
    setUAV("gOutputInstances", outputInstances_->GetResource()->GetGPUVirtualAddress());
    setUAV("gOutCommands", indirectCommandBuffer_->GetResource()->GetGPUVirtualAddress());
    setUAV("gCountBuffer", drawArgsBuffer_->GetResource()->GetGPUVirtualAddress());

    commandList->Dispatch((maxInstances + 63) / 64, 1, 1);

    currentBatchCBIndex_ = (currentBatchCBIndex_ + 1) % kMaxBatches;

    D3D12_RESOURCE_BARRIER postBarriers[3];
    postBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(outputInstances_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    postBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
    postBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(indirectCommandBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
    commandList->ResourceBarrier(3, postBarriers);
}

} // namespace Engine::Graphics
