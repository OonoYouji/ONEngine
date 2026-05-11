#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <d3d12.h>
#include "Engine/Graphics/Shader/PipelineState.h"
#include "Engine/Graphics/Shader/RootSignature.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// シェーダーとパイプラインステートを管理するクラス
///
class ShaderManager {
public:
    static ShaderManager& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new ShaderManager();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    bool LoadPipelineAsset(const std::string& filePath);

    PipelineState* GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& overrideDesc);
    PipelineState* GetOrCreateMeshPSO(const std::string& templateName, const PipelineStateDesc& overrideDesc);
    ID3D12PipelineState* GetComputePSO(const std::string& templateName);
    
    RootSignature* GetRootSignature(const std::string& name);
    ShaderObject* GetShader(const std::string& name);
    bool LoadShader(const std::string& name, const std::wstring& filePath, const std::wstring& entryPoint, const std::wstring& profile);

private:
    ShaderManager();
    ~ShaderManager();

    static ShaderManager* instance_;

    RenderDevice* device_ = nullptr;
    std::unique_ptr<ShaderCompiler> compiler_;

    std::unordered_map<std::string, std::unique_ptr<ShaderObject>> shaders_;
    std::unordered_map<std::string, std::unique_ptr<RootSignature>> rootSignatures_;
    std::unordered_map<std::string, std::unique_ptr<PipelineState>> pipelineStates_;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> computePipelineStates_;

    struct ShaderFileInfo {
        std::wstring path;
        std::wstring entry;
        std::wstring profile;
        bool isValid = false;
    };

    struct PipelineAsset {
        std::string name;
        ShaderFileInfo vs, ps, as, ms, cs;
        PipelineStateDesc baseDesc;
    };

    std::unordered_map<std::string, PipelineAsset> pipelineAssets_;

    std::string GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc);
};

} // namespace Engine::Graphics
