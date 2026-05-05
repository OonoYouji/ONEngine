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
    nameToParameterIndex_.clear();
    std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
    std::vector<std::unique_ptr<D3D12_DESCRIPTOR_RANGE1[]>> rangeArrays;

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

    for (const auto& data : reflectionDataList) {
        for (const auto& cb : data.constantBuffers) cbvs[{cb.bindPoint, cb.space}] = cb;
        for (const auto& srv : data.srvs) srvs[{srv.bindPoint, srv.space}] = srv;
        for (const auto& uav : data.uavs) uavs[{uav.bindPoint, uav.space}] = uav;
        for (const auto& sampler : data.samplers) samplers[{sampler.bindPoint, sampler.space}] = sampler;
    }

    std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

    auto addParam = [&](const auto& res, D3D12_DESCRIPTOR_RANGE_TYPE type) {
        if (type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {
            D3D12_ROOT_PARAMETER1 param = {};
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            param.Descriptor.ShaderRegister = res.bindPoint;
            param.Descriptor.RegisterSpace = res.space;
            param.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            nameToParameterIndex_[res.name] = static_cast<uint32_t>(rootParameters.size());
            Engine::Console::Log(std::format("RootSig Bind (CBV): {} -> Index {}", res.name, rootParameters.size()));
            rootParameters.push_back(param);
        }
        else if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
            // ... (s0, space0 の場合は静的サンプラーとして登録、それ以外は Descriptor Table)
            if (res.bindPoint == 0 && res.space == 0) {
                D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
                samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.MipLODBias = 0;
                samplerDesc.MaxAnisotropy = 1;
                samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
                samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                samplerDesc.MinLOD = 0.0f;
                samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
                samplerDesc.ShaderRegister = res.bindPoint;
                samplerDesc.RegisterSpace = res.space;
                samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                staticSamplers.push_back(samplerDesc);
                Engine::Console::Log(std::format("RootSig Static Sampler Bind: s{} (space{})", res.bindPoint, res.space));
            }
            else {
                auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE1[]>(1);
                ranges[0].RangeType = type;
                ranges[0].BaseShaderRegister = res.bindPoint;
                ranges[0].RegisterSpace = res.space;
                ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                ranges[0].NumDescriptors = res.bindCount;
                ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

                D3D12_ROOT_PARAMETER1 param = {};
                param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                param.DescriptorTable.NumDescriptorRanges = 1;
                param.DescriptorTable.pDescriptorRanges = ranges.get();
                param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

                nameToParameterIndex_[res.name] = static_cast<uint32_t>(rootParameters.size());
                Engine::Console::Log(std::format("RootSig Bind (Sampler Table): {} -> Index {}", res.name, rootParameters.size()));
                rootParameters.push_back(param);
                rangeArrays.push_back(std::move(ranges));
            }
        }
        else {
            // SRV, UAV (res は ShaderResourceInfo であることが確定)
            bool isTexture = (res.type == D3D_SIT_TEXTURE || res.type == D3D_SIT_UAV_RWTYPED);
            bool isBindless = (res.bindCount == 0 || res.bindCount == 0xFFFFFFFF);

            if (isBindless || isTexture) {
                auto ranges = std::make_unique<D3D12_DESCRIPTOR_RANGE1[]>(1);
                ranges[0].RangeType = type;
                ranges[0].BaseShaderRegister = res.bindPoint;
                ranges[0].RegisterSpace = res.space;
                ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                
                if (isBindless) {
                    ranges[0].NumDescriptors = 1024; // Bindless用暫定数
                    ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
                } else {
                    ranges[0].NumDescriptors = res.bindCount;
                    ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                }

                D3D12_ROOT_PARAMETER1 param = {};
                param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                param.DescriptorTable.NumDescriptorRanges = 1;
                param.DescriptorTable.pDescriptorRanges = ranges.get();
                param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

                nameToParameterIndex_[res.name] = static_cast<uint32_t>(rootParameters.size());
                Engine::Console::Log(std::format("RootSig Bind (Table): {} -> Index {}", res.name, rootParameters.size()));
                rootParameters.push_back(param);
                rangeArrays.push_back(std::move(ranges));
            }
            else {
                D3D12_ROOT_PARAMETER1 param = {};
                param.ParameterType = (type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) ? D3D12_ROOT_PARAMETER_TYPE_SRV : D3D12_ROOT_PARAMETER_TYPE_UAV;
                param.Descriptor.ShaderRegister = res.bindPoint;
                param.Descriptor.RegisterSpace = res.space;
                param.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
                param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

                nameToParameterIndex_[res.name] = static_cast<uint32_t>(rootParameters.size());
                Engine::Console::Log(std::format("RootSig Bind (Root SRV/UAV): {} -> Index {}", res.name, rootParameters.size()));
                rootParameters.push_back(param);
            }
        }
    };

    for (const auto& [key, cb] : cbvs) addParam(cb, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
    for (const auto& [key, srv] : srvs) addParam(srv, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
    for (const auto& [key, uav] : uavs) addParam(uav, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
    for (const auto& [key, sampler] : samplers) addParam(sampler, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSigDesc.Desc_1_1.NumParameters = static_cast<uint32_t>(rootParameters.size());
    rootSigDesc.Desc_1_1.pParameters = rootParameters.data();
    rootSigDesc.Desc_1_1.NumStaticSamplers = static_cast<uint32_t>(staticSamplers.size());
    rootSigDesc.Desc_1_1.pStaticSamplers = staticSamplers.data();
    rootSigDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &signature, &error);

    if (FAILED(hr)) {
        if (error) Console::Log(std::format("Root signature serialization error: {}", (const char*)error->GetBufferPointer()));
        return false;
    }

    hr = device->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    if (FAILED(hr)) {
        Console::Log("Failed to create root signature");
        return false;
    }

    return true;
}

uint32_t RootSignature::GetParameterIndex(const std::string& name) const {
    auto it = nameToParameterIndex_.find(name);
    if (it == nameToParameterIndex_.end()) {
        return kInvalidIndex;
    }
    return it->second;
}

} // namespace Engine::Graphics
