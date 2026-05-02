#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "ShaderCompiler.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "PipelineAsset.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// シェーダーとパイプライン状態を一括管理するクラス
///
class ShaderManager {
public:
    static ShaderManager& GetInstance() {
        static ShaderManager instance;
        return instance;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief パイプラインアセット（JSON）のロード
    /// @param filePath JSONファイルのパス
    bool LoadPipelineAsset(const std::string& filePath);

    /// @brief 指定したテンプレートと設定からPSOを取得または作成する
    /// @param templateName 登録済みのパイプラインアセット名
    /// @param desc 設定（オーバーライド可能）
    /// @return PSO
    PipelineState* GetOrCreatePSO(const std::string& templateName, const PipelineStateDesc& desc = PipelineStateDesc());

    /// @brief シェーダーのコンパイルと登録
    bool LoadShader(const std::string& name, const std::wstring& filePath, const std::wstring& entryPoint, const std::wstring& profile);

    /// @brief パイプラインの作成と登録
    /// @param name 登録名
    /// @param vsName 頂点シェーダー名
    /// @param psName ピクセルシェーダー名
    /// @param desc パイプライン設定
    bool CreatePipelineState(const std::string& name, const std::string& vsName, const std::string& psName, const PipelineStateDesc& desc = PipelineStateDesc());

    /// @brief メッシュシェーダーパイプラインの作成と登録
    bool CreateMeshShaderPipelineState(const std::string& name, const std::string& asName, const std::string& msName, const std::string& psName, const PipelineStateDesc& desc = PipelineStateDesc());

    /// @brief 登録済みのシェーダー取得
    ShaderObject* GetShader(const std::string& name);

    /// @brief 登録済みのパイプライン取得
    PipelineState* GetPipelineState(const std::string& name);

    /// @brief 登録済みのルートシグネチャ取得（基本的には名前空間やマテリアルごとに共有される想定）
    RootSignature* GetRootSignature(const std::string& name);

private:
    ShaderManager();
    ~ShaderManager();
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    /// @brief 設定から一意なハッシュキーを生成する
    std::string GeneratePSOKey(const std::string& templateName, const PipelineStateDesc& desc);

private:
    RenderDevice* device_ = nullptr;
    std::unique_ptr<ShaderCompiler> compiler_;

    std::unordered_map<std::string, PipelineAsset> pipelineAssets_;
    std::unordered_map<std::string, std::unique_ptr<ShaderObject>> shaders_;
    std::unordered_map<std::string, std::unique_ptr<RootSignature>> rootSignatures_;
    std::unordered_map<std::string, std::unique_ptr<PipelineState>> pipelineStates_;
};

} // namespace Engine::Graphics
