#include "ClusteredLightManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include <d3dx12.h>

namespace Engine::Graphics {

ClusteredLightManager::ClusteredLightManager() = default;
ClusteredLightManager::~ClusteredLightManager() = default;

void ClusteredLightManager::Initialize(RenderDevice* device) {
    device_ = device;

    clusterAABBBuffer_ = std::make_unique<StructuredBuffer>();
    clusterAABBBuffer_->Create(device, sizeof(Engine::Math::Vector4) * 2, kNumClusters, nullptr, true); // UAV

    lightGridBuffer_ = std::make_unique<StructuredBuffer>();
    lightGridBuffer_->Create(device, sizeof(uint32_t) * 2, kNumClusters, nullptr, true); // UAV

    // インデックスリスト: 全クラスタ * 最大ライト数
    lightIndexListBuffer_ = std::make_unique<StructuredBuffer>();
    lightIndexListBuffer_->Create(device, sizeof(uint32_t), kNumClusters * kMaxLightsPerCluster, nullptr, true); // UAV

    globalIndexCountBuffer_ = std::make_unique<StructuredBuffer>();
    globalIndexCountBuffer_->Create(device, sizeof(uint32_t), 1, nullptr, true); // UAV (Counter)

    clusterParamsCB_ = std::make_unique<ConstantBuffer>();
    clusterParamsCB_->Create(device, sizeof(ClusterParams));
}

void ClusteredLightManager::Shutdown() {
    clusterAABBBuffer_.reset();
    lightGridBuffer_.reset();
    lightIndexListBuffer_.reset();
    globalIndexCountBuffer_.reset();
    clusterParamsCB_.reset();
}

void ClusteredLightManager::BuildClusters(ID3D12GraphicsCommandList* commandList, const Engine::Math::Matrix4x4& invProj, const Engine::Math::Vector2Int& screenSize, float nearZ, float farZ) {
    auto& sm = ShaderManager::GetInstance();
    auto* pso = sm.GetComputePSO("ClusterBuilding");
    auto* rootSig = sm.GetRootSignature("ClusterBuilding");

    if (!pso || !rootSig) return;

    clusterAABBBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    ClusterParams params = {};
    params.invProj = invProj;
    params.nearZ = nearZ;
    params.farZ = farZ;
    params.screenWidth = (float)screenSize.x;
    params.screenHeight = (float)screenSize.y;
    clusterParamsCB_->Update(&params, sizeof(params));
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gClusters"), clusterAABBBuffer_->GetResource()->GetGPUVirtualAddress());

    // 各クラスタのZ面を構築するため、Z方向に Dispatch
    commandList->Dispatch(1, 1, kGridZ);

    // バリア: UAV -> SRV (Assignment pass で読み込むため)
    clusterAABBBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    clustersBuilt_ = true;
}

void ClusteredLightManager::AssignLights(ID3D12GraphicsCommandList* commandList, D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress, D3D12_GPU_VIRTUAL_ADDRESS pointLightBufferAddress, uint32_t totalLights) {
    if (!clustersBuilt_) return;

    auto& sm = ShaderManager::GetInstance();
    
    // 1. カウンタリセット (CullingReset を流用)
    auto* resetPSO = sm.GetComputePSO("CullingReset");
    auto* resetRootSig = sm.GetRootSignature("CullingReset");
    if (resetPSO && resetRootSig) {
        globalIndexCountBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->SetComputeRootSignature(resetRootSig->Get());
        commandList->SetPipelineState(resetPSO);
        commandList->SetComputeRootUnorderedAccessView(resetRootSig->GetParameterIndex("gCountBuffer"), globalIndexCountBuffer_->GetResource()->GetGPUVirtualAddress());
        commandList->Dispatch(1, 1, 1);
    }

    auto* pso = sm.GetComputePSO("LightAssignment");
    auto* rootSig = sm.GetRootSignature("LightAssignment");

    if (!pso || !rootSig) return;

    // パラメータ更新
    ClusterParams params = {};
    params.totalLights = totalLights;
    clusterParamsCB_->Update(&params, sizeof(params));

    lightGridBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    lightIndexListBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    globalIndexCountBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);

    commandList->SetComputeRootConstantBufferView(rootSig->GetParameterIndex("gParams"), clusterParamsCB_->GetGPUVirtualAddress());
    commandList->SetComputeRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), sceneCBAddress);
    commandList->SetComputeRootShaderResourceView(rootSig->GetParameterIndex("gClusters"), clusterAABBBuffer_->GetResource()->GetGPUVirtualAddress());
    commandList->SetComputeRootShaderResourceView(rootSig->GetParameterIndex("gPointLights"), pointLightBufferAddress);
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gLightGrid"), lightGridBuffer_->GetResource()->GetGPUVirtualAddress());
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gLightIndexList"), lightIndexListBuffer_->GetResource()->GetGPUVirtualAddress());
    commandList->SetComputeRootUnorderedAccessView(rootSig->GetParameterIndex("gGlobalIndexCount"), globalIndexCountBuffer_->GetResource()->GetGPUVirtualAddress());

    commandList->Dispatch(1, 1, kGridZ);

    // バリア: UAV -> SRV (メインパスで読み込むため)
    lightGridBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    lightIndexListBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

} // namespace Engine::Graphics
