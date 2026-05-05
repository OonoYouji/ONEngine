#pragma once

#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Engine/Graphics/Utils/ComPtr.h"
#include "ShaderCompiler.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "PipelineAsset.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// シェーダーとパイプライン状態を一括管理するクラス
/// ※キャッシュ回避のため一時的に名称を変更
///
class ShaderManager {
public:
    static ShaderManager& GetInstance() {
        static ShaderManager instance;
        return instance;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    bool LoadPipelineAsset(const std::string& filePath);

    // 確実に存在するはずのメソッド群
    PipelineState* GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& desc = PipelineStateDesc());
    ID3D12PipelineState* GetComputePSO(const std::string& templateName);
    PipelineState* GetOrCreateMeshPSO(const std::string& templateName, const PipelineStateDesc& desc = PipelineStateDesc());
    RootSignature* GetRootSignature(const std::string& name);
    ShaderObject* GetShader(const std::string& name);

private:
    ShaderManager();
    ~ShaderManager();
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    bool LoadShader(const std::string& name, const std::wstring& filePath, const std::wstring& entryPoint, const std::wstring& profile);
    std::string GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc);

private:
    RenderDevice* device_ = nullptr;
    std::unique_ptr<ShaderCompiler> compiler_;

    std::unordered_map<std::string, PipelineAsset> pipelineAssets_;
    std::unordered_map<std::string, std::unique_ptr<ShaderObject>> shaders_;
    std::unordered_map<std::string, std::unique_ptr<RootSignature>> rootSignatures_;
    std::unordered_map<std::string, std::unique_ptr<PipelineState>> pipelineStates_;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> computePipelineStates_;
};

} // namespace Engine::Graphics
