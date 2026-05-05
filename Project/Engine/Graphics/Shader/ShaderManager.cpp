#include "ShaderManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include <d3dx12.h>

using json = nlohmann::json;

namespace Engine::Graphics {

ShaderManager::ShaderManager() = default;
ShaderManager::~ShaderManager() = default;

void ShaderManager::Initialize(RenderDevice* device) {
    device_ = device;
    compiler_ = std::make_unique<ShaderCompiler>();
    compiler_->Initialize();
}

void ShaderManager::Shutdown() {
    shaders_.clear();
    rootSignatures_.clear();
    pipelineStates_.clear();
    pipelineAssets_.clear();
    computePipelineStates_.clear();
    compiler_.reset();
}

bool ShaderManager::LoadPipelineAsset(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    try {
        json data = json::parse(file);
        PipelineAsset asset;
        asset.name = data.value("name", "");

        auto parseShader = [](const json& j, ShaderFileInfo& info) {
            if (j.is_object()) {
                info.path = Engine::ConvertString(j.value("path", ""));
                info.entry = Engine::ConvertString(j.value("entry", ""));
                info.profile = Engine::ConvertString(j.value("profile", ""));
                info.isValid = !info.path.empty();
            }
        };

        parseShader(data["vs"], asset.vs);
        parseShader(data["ps"], asset.ps);
        parseShader(data["as"], asset.as);
        parseShader(data["ms"], asset.ms);
        parseShader(data["cs"], asset.cs);

        auto& desc = asset.baseDesc;
        if (data.contains("rasterizer")) {
            auto& r = data["rasterizer"];
            std::string cull = r.value("cull", "BACK");
            if (cull == "NONE") desc.cullMode = D3D12_CULL_MODE_NONE;
            else if (cull == "FRONT") desc.cullMode = D3D12_CULL_MODE_FRONT;
            else desc.cullMode = D3D12_CULL_MODE_BACK;

            std::string fill = r.value("fill", "SOLID");
            if (fill == "WIREFRAME") desc.fillMode = D3D12_FILL_MODE_WIREFRAME;
            else desc.fillMode = D3D12_FILL_MODE_SOLID;
        }

        if (data.contains("depth")) {
            auto& d = data["depth"];
            desc.depthEnable = d.value("enable", true);
            desc.depthWriteEnable = d.value("write", true);
            std::string func = d.value("func", "LESS");
            if (func == "LESS_EQUAL") desc.depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            else if (func == "EQUAL") desc.depthFunc = D3D12_COMPARISON_FUNC_EQUAL;
            else desc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        }

        if (data.contains("blend")) {
            desc.blendEnable = data["blend"].value("enable", false);
        }

        if (asset.name.empty()) asset.name = std::filesystem::path(filePath).stem().string();

        pipelineAssets_[asset.name] = asset;
        return true;
    } catch (...) { return false; }
}

PipelineState* ShaderManager::GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& overrideDesc) {
    auto assetIt = pipelineAssets_.find(templateName);
    if (assetIt == pipelineAssets_.end()) return nullptr;
    const auto& asset = assetIt->second;

    PipelineStateDesc finalDesc = asset.baseDesc;
    finalDesc.usePS = overrideDesc.usePS;
    finalDesc.numRenderTargets = overrideDesc.numRenderTargets;
    finalDesc.rtvFormat = overrideDesc.rtvFormat;
    finalDesc.dsvFormat = overrideDesc.dsvFormat;
    finalDesc.primitiveTopologyType = overrideDesc.primitiveTopologyType;

    if (!overrideDesc.usePS) {
        finalDesc.depthWriteEnable = true;
        finalDesc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        finalDesc.numRenderTargets = 0;
    } else if (overrideDesc.depthFunc == D3D12_COMPARISON_FUNC_EQUAL) {
        finalDesc.depthFunc = D3D12_COMPARISON_FUNC_EQUAL;
        finalDesc.depthWriteEnable = false;
    }

    std::string key = GeneratePSOKey(templateName, finalDesc);
    if (pipelineStates_.count(key)) return pipelineStates_[key].get();

    auto ensureShader = [&](const std::string& shaderKey, const ShaderFileInfo& info) {
        if (!info.isValid) return (ShaderObject*)nullptr;
        if (!GetShader(shaderKey)) LoadShader(shaderKey, info.path, info.entry, info.profile);
        return GetShader(shaderKey);
    };

    ShaderObject* vs = ensureShader(templateName + "_VS", asset.vs);
    ShaderObject* ps = ensureShader(templateName + "_PS", asset.ps);

    if (!rootSignatures_.count(templateName)) {
        std::vector<ShaderReflectionData> reflections;
        if (vs) reflections.push_back(vs->reflectionData);
        if (ps) reflections.push_back(ps->reflectionData);
        auto rootSig = std::make_unique<RootSignature>();
        if (!rootSig->Create(device_, reflections)) return nullptr;
        rootSignatures_[templateName] = std::move(rootSig);
    }
    RootSignature* rootSig = rootSignatures_[templateName].get();

    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSig, vs, finalDesc.usePS ? ps : nullptr, nullptr, nullptr, finalDesc)) return nullptr;

    pipelineStates_[key] = std::move(pso);
    return pipelineStates_[key].get();
}

ID3D12PipelineState* ShaderManager::GetComputePSO(const std::string& templateName) {
    if (computePipelineStates_.count(templateName)) return computePipelineStates_[templateName].Get();

    auto assetIt = pipelineAssets_.find(templateName);
    if (assetIt == pipelineAssets_.end() || !assetIt->second.cs.isValid) return nullptr;
    const auto& asset = assetIt->second;

    ShaderObject* cs = GetShader(templateName + "_CS");
    if (!cs) {
        if (!LoadShader(templateName + "_CS", asset.cs.path, asset.cs.entry, asset.cs.profile)) return nullptr;
        cs = GetShader(templateName + "_CS");
    }

    if (!rootSignatures_.count(templateName)) {
        auto rootSig = std::make_unique<RootSignature>();
        if (!rootSig->Create(device_, { cs->reflectionData })) return nullptr;
        rootSignatures_[templateName] = std::move(rootSig);
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignatures_[templateName]->Get();
    psoDesc.CS = { cs->blob->GetBufferPointer(), cs->blob->GetBufferSize() };

    ComPtr<ID3D12PipelineState> pso;
    device_->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pso));
    computePipelineStates_[templateName] = pso;
    return pso.Get();
}

PipelineState* ShaderManager::GetOrCreateMeshPSO(const std::string& templateName, const PipelineStateDesc& overrideDesc) {
    auto assetIt = pipelineAssets_.find(templateName);
    if (assetIt == pipelineAssets_.end()) return nullptr;
    const auto& asset = assetIt->second;

    PipelineStateDesc finalDesc = asset.baseDesc;
    finalDesc.rtvFormat = overrideDesc.rtvFormat;
    finalDesc.dsvFormat = overrideDesc.dsvFormat;

    std::string key = "Mesh_" + GeneratePSOKey(templateName, finalDesc);
    if (pipelineStates_.count(key)) return pipelineStates_[key].get();

    auto ensureShader = [&](const std::string& shaderKey, const ShaderFileInfo& info) {
        if (!info.isValid) return (ShaderObject*)nullptr;
        if (!GetShader(shaderKey)) LoadShader(shaderKey, info.path, info.entry, info.profile);
        return GetShader(shaderKey);
    };

    ShaderObject* as = ensureShader(templateName + "_AS", asset.as);
    ShaderObject* ms = ensureShader(templateName + "_MS", asset.ms);
    ShaderObject* ps = ensureShader(templateName + "_PS", asset.ps);

    if (!rootSignatures_.count(templateName)) {
        std::vector<ShaderReflectionData> reflections;
        if (as) reflections.push_back(as->reflectionData);
        if (ms) reflections.push_back(ms->reflectionData);
        if (ps) reflections.push_back(ps->reflectionData);
        auto rootSig = std::make_unique<RootSignature>();
        if (!rootSig->Create(device_, reflections)) return nullptr;
        rootSignatures_[templateName] = std::move(rootSig);
    }

    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSignatures_[templateName].get(), nullptr, ps, as, ms, finalDesc)) return nullptr;

    pipelineStates_[key] = std::move(pso);
    return pipelineStates_[key].get();
}

std::string ShaderManager::GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc) {
    return std::format("{}_{}_{}_{}_{}_{}_{}_{}_{}_{}_{}",
        templateName, (int)desc.cullMode, (int)desc.fillMode, desc.depthEnable,
        desc.depthWriteEnable, (int)desc.depthFunc, desc.blendEnable,
        (int)desc.rtvFormat, (int)desc.dsvFormat, desc.usePS, (int)desc.primitiveTopologyType);
}

bool ShaderManager::LoadShader(const std::string& name, const std::wstring& filePath, const std::wstring& entryPoint, const std::wstring& profile) {
    auto shader = compiler_->Compile(filePath, entryPoint, profile);
    if (!shader) return false;
    shaders_[name] = std::move(shader);
    return true;
}

ShaderObject* ShaderManager::GetShader(const std::string& name) {
    auto it = shaders_.find(name);
    return (it != shaders_.end()) ? it->second.get() : nullptr;
}

RootSignature* ShaderManager::GetRootSignature(const std::string& name) {
    auto it = rootSignatures_.find(name);
    return (it != rootSignatures_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Graphics
