#pragma once

#include <string>
#include <vector>
#include <memory>
#include <d3d12.h>
#include <dxcapi.h>

#include "Engine/Graphics/Utils/ComPtr.h"
#include "ShaderReflectionData.h"

namespace Engine::Graphics {

///
/// コンパイル済みのシェーダーオブジェクト
///
struct ShaderObject {
    ComPtr<IDxcBlob> blob;
    ShaderReflectionData reflectionData;
};

///
/// DXCを使用したシェーダーコンパイラ
///
class ShaderCompiler {
public:
    ShaderCompiler();
    ~ShaderCompiler();

    /// @brief 初期化
    void Initialize();

    /// @brief シェーダーファイルのコンパイル
    /// @param filePath ファイルパス
    /// @param entryPoint エントリポイント
    /// @param profile プロファイル (vs_6_0, ps_6_0等)
    /// @return コンパイル結果。失敗した場合は nullptr。
    std::unique_ptr<ShaderObject> Compile(
        const std::wstring& filePath,
        const std::wstring& entryPoint,
        const std::wstring& profile
    );

private:
    /// @brief 反射情報の抽出
    void ExtractReflection(IDxcBlob* reflectionBlob, ShaderReflectionData& outData);

private:
    ComPtr<IDxcUtils> utils_;
    ComPtr<IDxcCompiler3> compiler_;
    ComPtr<IDxcIncludeHandler> includeHandler_;
};

} // namespace Engine::Graphics
