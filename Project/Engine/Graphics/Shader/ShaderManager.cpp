#include "ShaderManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

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
    compiler_.reset();
}

bool ShaderManager::LoadPipelineAsset(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("Failed to open pipeline asset: {}", filePath));
        return false;
    }

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

        if (asset.name.empty()) {
            asset.name = std::filesystem::path(filePath).stem().string();
        }

        pipelineAssets_[asset.name] = asset;
        Engine::Console::Log(std::format("Loaded Pipeline Asset: {}", asset.name));
        return true;
    }
    catch (const std::exception& e) {
        Engine::Console::LogError(std::format("Failed to parse pipeline asset: {}\n{}", filePath, e.what()));
        return false;
    }
}

PipelineState* ShaderManager::GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& desc) {
    // 1. キャッシュキーの生成
    std::string key = GeneratePSOKey(templateName, desc);
    if (pipelineStates_.count(key)) {
        return pipelineStates_[key].get();
    }

    // 2. テンプレートの取得
    auto assetIt = pipelineAssets_.find(templateName);
    if (assetIt == pipelineAssets_.end()) {
        Engine::Console::LogError(std::format("Pipeline template not found: {}", templateName));
        return nullptr;
    }
    const auto& asset = assetIt->second;

    // 3. 必要に応じてシェーダーをロード（既にロード済みならスキップされる）
    auto ensureShader = [&](const std::string& shaderKey, const ShaderFileInfo& info) {
        if (!info.isValid) return (ShaderObject*)nullptr;
        if (!GetShader(shaderKey)) {
            if (!LoadShader(shaderKey, info.path, info.entry, info.profile)) {
                Engine::Console::LogError(std::format("Failed to compile shader: {} (Path: {})", shaderKey, Engine::ConvertString(info.path)));
            }
        }
        return GetShader(shaderKey);
    };

    // シェーダーのキー名は "TemplateName_VS" のようにして管理
    ShaderObject* vs = ensureShader(templateName + "_VS", asset.vs);
    ShaderObject* ps = ensureShader(templateName + "_PS", asset.ps);
    ShaderObject* as = ensureShader(templateName + "_AS", asset.as);
    ShaderObject* ms = ensureShader(templateName + "_MS", asset.ms);

    if (!ps || (!vs && !ms)) {
        Engine::Console::LogError(std::format("Failed to ensure shaders for template: {}", templateName));
        return nullptr;
    }

    // 4. ルートシグネチャの作成または取得（テンプレートごとに1つ）
    if (!rootSignatures_.count(templateName)) {
        std::vector<ShaderReflectionData> reflections;
        if (vs) reflections.push_back(vs->reflectionData);
        if (ps) reflections.push_back(ps->reflectionData);
        if (as) reflections.push_back(as->reflectionData);
        if (ms) reflections.push_back(ms->reflectionData);

        auto rootSig = std::make_unique<RootSignature>();
        if (!rootSig->Create(device_, reflections)) return nullptr;
        rootSignatures_[templateName] = std::move(rootSig);
    }
    RootSignature* rootSig = rootSignatures_[templateName].get();

    // 5. PSOの作成
    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSig, vs, ps, as, ms, desc)) {
        return nullptr;
    }

    pipelineStates_[key] = std::move(pso);
    return pipelineStates_[key].get();
}

std::string ShaderManager::GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc) {
    // 設定値を文字列連結して簡易的なハッシュキーとする
    return std::format("{}_{}_{}_{}_{}_{}_{}_{}",
        templateName,
        (int)desc.cullMode,
        (int)desc.fillMode,
        desc.depthEnable,
        desc.depthWriteEnable,
        (int)desc.depthFunc,
        desc.blendEnable,
        (int)desc.rtvFormat
    );
}

bool ShaderManager::LoadShader(
    const std::string& name,
    const std::wstring& filePath,
    const std::wstring& entryPoint,
    const std::wstring& profile
) {
    auto shader = compiler_->Compile(filePath, entryPoint, profile);
    if (!shader) {
        return false;
    }

    shaders_[name] = std::move(shader);
    return true;
}

bool ShaderManager::CreatePipelineState(
    const std::string& name,
    const std::string& vsName,
    const std::string& psName,
    const PipelineStateDesc& desc
) {
    auto* vs = GetShader(vsName);
    auto* ps = GetShader(psName);

    if (!vs || !ps) {
        Engine::Console::LogError("Failed to create PipelineState: Shader not found.");
        return false;
    }

    auto rootSig = std::make_unique<RootSignature>();
    if (!rootSig->Create(device_, { vs->reflectionData, ps->reflectionData })) {
        return false;
    }

    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSig.get(), vs, ps, nullptr, nullptr, desc)) {
        return false;
    }

    rootSignatures_[name] = std::move(rootSig);
    pipelineStates_[name] = std::move(pso);

    return true;
}

bool ShaderManager::CreateMeshShaderPipelineState(
    const std::string& name,
    const std::string& asName,
    const std::string& msName,
    const std::string& psName,
    const PipelineStateDesc& desc
) {
    auto* as = asName.empty() ? nullptr : GetShader(asName);
    auto* ms = GetShader(msName);
    auto* ps = GetShader(psName);

    if (!ms || !ps) {
        Engine::Console::LogError("Failed to create MeshShader PipelineState: Shader not found.");
        return false;
    }

    std::vector<ShaderReflectionData> reflections;
    if (as) reflections.push_back(as->reflectionData);
    reflections.push_back(ms->reflectionData);
    reflections.push_back(ps->reflectionData);

    auto rootSig = std::make_unique<RootSignature>();
    if (!rootSig->Create(device_, reflections)) {
        return false;
    }

    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSig.get(), nullptr, ps, as, ms, desc)) {
        return false;
    }

    rootSignatures_[name] = std::move(rootSig);
    pipelineStates_[name] = std::move(pso);

    return true;
}

ShaderObject* ShaderManager::GetShader(const std::string& name) {
    auto it = shaders_.find(name);
    return (it != shaders_.end()) ? it->second.get() : nullptr;
}

PipelineState* ShaderManager::GetPipelineState(const std::string& name) {
    auto it = pipelineStates_.find(name);
    return (it != pipelineStates_.end()) ? it->second.get() : nullptr;
}

RootSignature* ShaderManager::GetRootSignature(const std::string& name) {
    auto it = rootSignatures_.find(name);
    return (it != rootSignatures_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Graphics
