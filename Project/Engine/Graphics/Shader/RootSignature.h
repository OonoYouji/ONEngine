#pragma once

#include <d3d12.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "Engine/Graphics/Utils/ComPtr.h"
#include "ShaderReflectionData.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// D3D12 ルートシグネチャを管理するクラス
///
class RootSignature {
public:
    RootSignature();
    ~RootSignature();

    /// @brief 反射情報からルートシグネチャを構築
    /// @param device レンダリングデバイス
    /// @param reflectionData シェーダーの反射情報（複数シェーダー分を統合可能）
    bool Create(RenderDevice* device, const std::vector<ShaderReflectionData>& reflectionDataList);

    ID3D12RootSignature* Get() const { return rootSignature_.Get(); }

    /// @brief 名前からルートパラメータのインデックスを取得
    uint32_t GetParameterIndex(const std::string& name) const;

private:
    ComPtr<ID3D12RootSignature> rootSignature_;
    std::unordered_map<std::string, uint32_t> nameToParameterIndex_;
};

} // namespace Engine::Graphics
