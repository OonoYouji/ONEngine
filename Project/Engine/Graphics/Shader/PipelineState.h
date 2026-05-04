#pragma once

#include <d3d12.h>
#include <string>
#include <memory>
#include <vector>

#include "Engine/Graphics/Utils/ComPtr.h"
#include "ShaderCompiler.h"
#include "RootSignature.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// パイプライン状態の設定（ストリーム形式対応）
///
struct PipelineStateDesc {
    D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
    D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;
    bool depthEnable = true;
    bool depthWriteEnable = true;
    D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS;
    bool blendEnable = false;
    D3D12_BLEND_DESC blendDesc = {};

    // レンダーターゲット設定
    DXGI_FORMAT rtvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Z-Prepass対応
    bool usePS = true;
    uint32_t numRenderTargets = 1;

    PipelineStateDesc() {

        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        for (auto& rt : blendDesc.RenderTarget) {
            rt.BlendEnable = FALSE;
            rt.SrcBlend = D3D12_BLEND_ONE;
            rt.DestBlend = D3D12_BLEND_ZERO;
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            rt.LogicOpEnable = FALSE;
            rt.LogicOp = D3D12_LOGIC_OP_NOOP;
            rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }
    }
};

///
/// ID3D12PipelineState を管理するクラス (Mesh Shader 対応版)
///
class PipelineState {
public:
    PipelineState();
    ~PipelineState();

    /// @brief パイプラインの作成
    /// @param device デバイス
    /// @param rootSignature ルートシグネチャ
    /// @param vs 頂点シェーダー (Mesh Shader 使用時は nullptr)
    /// @param ps ピクセルシェーダー
    /// @param as 増幅シェーダー (オプション)
    /// @param ms メッシュシェーダー (オプション)
    /// @param desc パイプライン設定
    bool Create(
        RenderDevice* device,
        RootSignature* rootSignature,
        ShaderObject* vs,
        ShaderObject* ps,
        ShaderObject* as = nullptr,
        ShaderObject* ms = nullptr,
        const PipelineStateDesc& desc = PipelineStateDesc()
    );

    ID3D12PipelineState* Get() const { return pipelineState_.Get(); }

private:
    ComPtr<ID3D12PipelineState> pipelineState_;
};

} // namespace Engine::Graphics
