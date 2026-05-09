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
    indirectCommandBuffer_->Create(device, sizeof(DrawIndexedArguments), kMaxBatches, nullptr, true); // バッチごとに1コマンド

    drawArgsBuffer_ = std::make_unique<StructuredBuffer>();
    drawArgsBuffer_->Create(device, sizeof(uint32_t), kMaxBatches, nullptr, true);

    frustumCB_ = std::make_unique<ConstantBuffer>();
    frustumCB_->Create(device, sizeof(FrustumPlanes));

    for (uint32_t i = 0; i < kMaxBatches; ++i) {
        cullingParamsCBs_[i] = std::make_unique<ConstantBuffer>();
        cullingParamsCBs_[i]->Create(device, sizeof(CullingParams) + 4 /* forceVisible */);
        
        buildParamsCBs_[i] = std::make_unique<ConstantBuffer>();
        buildParamsCBs_[i]->Create(device, 16); // BuildParams (16bytes)
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
        buildParamsCBs_[i].reset();
    }
    commandSignature_.Reset();
}

void GPUCullingManager::CreateCommandSignature() {
    D3D12_INDIRECT_ARGUMENT_DESC args[1] = {};
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

    D3D12_COMMAND_SIGNATURE_DESC desc = {};
    desc.ByteStride = sizeof(DrawIndexedArguments);
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

    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(1, &barrier);
    }

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gCountBuffer"), drawArgsBuffer_->GetResource()->GetGPUVirtualAddress());

    commandList->Dispatch(1, 1, 1); 

    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        commandList->ResourceBarrier(1, &barrier);
    }
}

void GPUCullingManager::Execute(ID3D12GraphicsCommandList* commandList, 
                              D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress,
                              StructuredBuffer* inputInstances,
                              uint32_t maxInstances,
                              const Engine::Math::Matrix4x4& vp,
                              D3D12_GPU_VIRTUAL_ADDRESS meshInfoBufferAddress,
                              uint32_t targetModelIndex,
                              uint32_t subMeshIndex,
                              uint32_t instanceOffset,
                              uint32_t batchIndex) {
    auto& sm = ShaderManager::GetInstance();
    auto* cullingPSO = sm.GetComputePSO("Culling");
    auto* cullingRootSig = sm.GetRootSignature("Culling");
    auto* buildPSO = sm.GetComputePSO("BuildCommands");
    auto* buildRootSig = sm.GetRootSignature("BuildCommands");

    if (!cullingPSO || !cullingRootSig || !buildPSO || !buildRootSig) return;

    // 1. Frustum Planes
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

    // 2. Parameters
    auto& cullingCB = cullingParamsCBs_[currentBatchCBIndex_];
    uint32_t cParams[6] = { targetModelIndex, maxInstances, instanceOffset, batchIndex, 1, subMeshIndex }; 
    cullingCB->Update(cParams, sizeof(cParams));

    auto& buildCB = buildParamsCBs_[currentBatchCBIndex_];
    uint32_t bParams[4] = { targetModelIndex, batchIndex, subMeshIndex, 0 };
    buildCB->Update(bParams, sizeof(bParams));

    // 3. Culling Pass
    {
        D3D12_RESOURCE_BARRIER barriers[3];
        barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(outputInstances_->GetResource(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        barriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(indirectCommandBuffer_->GetResource(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(3, barriers);
    }

    commandList->SetComputeRootSignature(cullingRootSig->Get());
    commandList->SetPipelineState(cullingPSO);

    auto setParam = [&](RootSignature* sig, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr, bool isUAV = false, bool isSRV = false) {
        auto idx = sig->GetParameterIndex(name);
        if (idx == RootSignature::kInvalidIndex) return;
        if (isUAV) commandList->SetComputeRootUnorderedAccessView(idx, addr);
        else if (isSRV) commandList->SetComputeRootShaderResourceView(idx, addr);
        else commandList->SetComputeRootConstantBufferView(idx, addr);
    };

    setParam(cullingRootSig, "gSceneData", sceneCBAddress);
    setParam(cullingRootSig, "gFrustum", frustumCB_->GetGPUVirtualAddress());
    setParam(cullingRootSig, "gCullingParams", cullingCB->GetGPUVirtualAddress());
    setParam(cullingRootSig, "gInputInstances", inputInstances->GetResource()->GetGPUVirtualAddress(), false, true);
    setParam(cullingRootSig, "gMeshInfos", meshInfoBufferAddress, false, true);
    setParam(cullingRootSig, "gOutputInstances", outputInstances_->GetResource()->GetGPUVirtualAddress(), true);
    setParam(cullingRootSig, "gCountBuffer", drawArgsBuffer_->GetResource()->GetGPUVirtualAddress(), true);

    commandList->Dispatch((maxInstances + 63) / 64, 1, 1);

    // 4. Build Commands Pass
    {
        D3D12_RESOURCE_BARRIER barriers[2];
        barriers[0] = CD3DX12_RESOURCE_BARRIER::UAV(drawArgsBuffer_->GetResource());
        barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(2, barriers);
    }

    commandList->SetComputeRootSignature(buildRootSig->Get());
    commandList->SetPipelineState(buildPSO);

    setParam(buildRootSig, "gParams", buildCB->GetGPUVirtualAddress());
    setParam(buildRootSig, "gMeshInfos", meshInfoBufferAddress, false, true);
    setParam(buildRootSig, "gCountBuffer", drawArgsBuffer_->GetResource()->GetGPUVirtualAddress(), false, true);
    setParam(buildRootSig, "gOutCommands", indirectCommandBuffer_->GetResource()->GetGPUVirtualAddress(), true);

    commandList->Dispatch(1, 1, 1);

    // 5. Post Barriers
    {
        D3D12_RESOURCE_BARRIER postBarriers[3];
        postBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(outputInstances_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        postBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(drawArgsBuffer_->GetResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        postBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(indirectCommandBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        commandList->ResourceBarrier(3, postBarriers);
    }

    currentBatchCBIndex_ = (currentBatchCBIndex_ + 1) % kMaxBatches;
}

} // namespace Engine::Graphics
