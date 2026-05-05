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

        // --- Render States ---
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
            else if (func == "ALWAYS") desc.depthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            else desc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        }

        if (data.contains("blend")) {
            desc.blendEnable = data["blend"].value("enable", false);
        }

        if (asset.name.empty()) {
            asset.name = std::filesystem::path(filePath).stem().string();
        }

        pipelineAssets_[asset.name] = asset;
        Engine::Console::Log(std::format("Loaded Pipeline Asset: {} (Cull:{}, DepthWrite:{}, Blend:{})", 
            asset.name, (int)desc.cullMode, desc.depthWriteEnable, desc.blendEnable));
        return true;
    }
    catch (const std::exception& e) {
        Engine::Console::LogError(std::format("Failed to parse pipeline asset: {}\n{}", filePath, e.what()));
        return false;
    }
}

PipelineState* ShaderManager::GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& overrideDesc) {
    // 2. テンプレートの取得
    auto assetIt = pipelineAssets_.find(templateName);
    if (assetIt == pipelineAssets_.end()) {
        Engine::Console::LogError(std::format("Pipeline template not found: {}", templateName));
        return nullptr;
    }
    const auto& asset = assetIt->second;

    // テンプレートの設定をベースに、渡された設定をマージする
    PipelineStateDesc finalDesc = asset.baseDesc;
    
    // 特定のフィールドをマージ（上書き）
    // ルール: JSONに定義があればそれを優先するが、Pass固有の設定（usePSやrtvFormat）は overrideDesc を優先する
    finalDesc.usePS = overrideDesc.usePS;
    finalDesc.numRenderTargets = overrideDesc.numRenderTargets;
    finalDesc.rtvFormat = overrideDesc.rtvFormat;
    finalDesc.dsvFormat = overrideDesc.dsvFormat;
    finalDesc.primitiveTopologyType = overrideDesc.primitiveTopologyType;

    // 深度書き込み設定は、マテリアル側が書き込みONの場合のみ、Pass側でOFFにできる（逆は不可など）
    // とりあえずシンプルに overrideDesc に値がセットされていれば（デフォルトでないなら）上書き
    // しかし C++ の構造体に "セットされているか" の判別は難しいので、
    // ここでは「overrideDesc のデフォルト値でないものだけ上書き」等の処理が必要。
    
    // 暫定: Z-Prepass (usePS=false) の時は強制的に深度書き込みONにするなどのロジックを入れる
    if (!overrideDesc.usePS) {
        finalDesc.depthWriteEnable = true;
        finalDesc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        finalDesc.numRenderTargets = 0;
    } else if (overrideDesc.depthFunc == D3D12_COMPARISON_FUNC_EQUAL) {
        // メインパス (EQUAL) の時は、テンプレート側の設定を維持しつつ func だけ EQUAL にする
        finalDesc.depthFunc = D3D12_COMPARISON_FUNC_EQUAL;
        finalDesc.depthWriteEnable = false;
    }

    // キャッシュキーの生成 (finalDesc を使用)
    std::string key = GeneratePSOKey(templateName, finalDesc);
    if (pipelineStates_.count(key)) {
        return pipelineStates_[key].get();
    }

    // シェーダーのロード
    auto ensureShader = [&](const std::string& shaderKey, const ShaderFileInfo& info) {
        if (!info.isValid) return (ShaderObject*)nullptr;
        if (!GetShader(shaderKey)) {
            if (!LoadShader(shaderKey, info.path, info.entry, info.profile)) {
                Engine::Console::LogError(std::format("Failed to compile shader: {} (Path: {})", shaderKey, Engine::ConvertString(info.path)));
            }
        }
        return GetShader(shaderKey);
    };

    ShaderObject* vs = ensureShader(templateName + "_VS", asset.vs);
    ShaderObject* ps = ensureShader(templateName + "_PS", asset.ps);
    ShaderObject* as = ensureShader(templateName + "_AS", asset.as);
    ShaderObject* ms = ensureShader(templateName + "_MS", asset.ms);

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

    ShaderObject* psForPSO = finalDesc.usePS ? ps : nullptr;
    auto pso = std::make_unique<PipelineState>();
    if (!pso->Create(device_, rootSig, vs, psForPSO, as, ms, finalDesc)) {
        return nullptr;
    }

    pipelineStates_[key] = std::move(pso);
    return pipelineStates_[key].get();
}

std::string ShaderManager::GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc) {
    return std::format("{}_{}_{}_{}_{}_{}_{}_{}_{}_{}_{}_{}",
        templateName,
        (int)desc.cullMode,
        (int)desc.fillMode,
        desc.depthEnable,
        desc.depthWriteEnable,
        (int)desc.depthFunc,
        desc.blendEnable,
        (int)desc.rtvFormat,
        (int)desc.dsvFormat,
        desc.usePS,
        desc.numRenderTargets,
        (int)desc.primitiveTopologyType
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
