#include "RootSignature.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

#include <map>
#include <algorithm>

namespace Engine::Graphics {

RootSignature::RootSignature() = default;
RootSignature::~RootSignature() = default;

bool RootSignature::Create(RenderDevice* device, const std::vector<ShaderReflectionData>& reflectionDataList) {
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    std::vector<std::unique_ptr<D3D12_DESCRIPTOR_RANGE[]>> rangeArrays; // メモリ確保用

    // 重複を避けるためにバインドポイントで管理
    // キー: (bindPoint, registerSpace), 値: リソース情報
    struct ResourceKey {
        uint32_t bindPoint;
        uint32_t space;
        bool operator<(const ResourceKey& other) const {
            if (space != other.space) return space < other.space;
            return bindPoint < other.bindPoint;
        }
    };

    std::map<ResourceKey, ShaderConstantBufferInfo> cbvs;
    std::map<ResourceKey, ShaderResourceInfo> srvs;
    std::map<ResourceKey, ShaderResourceInfo> uavs;
    std::map<ResourceKey, ShaderResourceInfo> samplers;

    // 1. 全シェーダーの情報を収集
    for (const auto& data : reflectionDataList) {
        for (const auto& cb : data.constantBuffers) cbvs[{cb.bindPoint, cb.space}] = cb;
        for (const auto& srv : data.srvs) srvs[{srv.bindPoint, srv.space}] = srv;
        for (const auto& uav : data.uavs) uavs[{uav.bindPoint, uav.space}] = uav;
        for (const auto& sampler : data.samplers) samplers[{sampler.bindPoint, sampler.space}] = sampler;
    }

    // 2. ルートパラメータの構築 (CBV は DescriptorTable ではなく直接 ConstantBufferView にする設計も可能だが、
    //    今回は汎用性を考え DescriptorTable を使用する)

    // CBVs
    for (const auto& [key, cb] : cbvs) {
        auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE[]>(1);
        ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        ranges[0].NumDescriptors = cb.bindCount;
        ranges[0].BaseShaderRegister = cb.bindPoint;
        ranges[0].RegisterSpace = cb.space;
        ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = 1;
        param.DescriptorTable.pDescriptorRanges = ranges.get();
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameters.push_back(param);
        rangeArrays.push_back(std::move(ranges));
    }

    // SRVs
    for (const auto& [key, srv] : srvs) {
        auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE[]>(1);
        ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        ranges[0].NumDescriptors = srv.bindCount;
        ranges[0].BaseShaderRegister = srv.bindPoint;
        ranges[0].RegisterSpace = srv.space;
        ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = 1;
        param.DescriptorTable.pDescriptorRanges = ranges.get();
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameters.push_back(param);
        rangeArrays.push_back(std::move(ranges));
    }

    // UAVs
    for (const auto& [key, uav] : uavs) {
        auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE[]>(1);
        ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        ranges[0].NumDescriptors = uav.bindCount;
        ranges[0].BaseShaderRegister = uav.bindPoint;
        ranges[0].RegisterSpace = uav.space;
        ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = 1;
        param.DescriptorTable.pDescriptorRanges = ranges.get();
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameters.push_back(param);
        rangeArrays.push_back(std::move(ranges));
    }

    // Samplers
    for (const auto& [key, sampler] : samplers) {
        auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE[]>(1);
        ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        ranges[0].NumDescriptors = sampler.bindCount;
        ranges[0].BaseShaderRegister = sampler.bindPoint;
        ranges[0].RegisterSpace = sampler.space;
        ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = 1;
        param.DescriptorTable.pDescriptorRanges = ranges.get();
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameters.push_back(param);
        rangeArrays.push_back(std::move(ranges));
    }

    // 3. ルートシグネチャのシリアライズと作成
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = static_cast<uint32_t>(rootParameters.size());
    rootSigDesc.pParameters = rootParameters.data();
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    if (FAILED(hr)) {
        if (error) {
            Console::Log(std::format("Root signature serialization error: {}", (const char*)error->GetBufferPointer()));
        }
        return false;
    }

    hr = device->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    if (FAILED(hr)) {
        Console::Log("Failed to create root signature");
        return false;
    }

    return true;
}

} // namespace Engine::Graphics
