#pragma once

#include <string>
#include "PipelineState.h"

namespace Engine::Graphics {

///
/// シェーダーファイルの情報を保持する構造体
///
struct ShaderFileInfo {
    std::wstring path;
    std::wstring entry;
    std::wstring profile;
    bool isValid = false;
};

///
/// パイプラインアセット（テンプレート）の情報
///
struct PipelineAsset {
    std::string name;
    ShaderFileInfo vs;
    ShaderFileInfo ps;
    ShaderFileInfo as; // Optional
    ShaderFileInfo ms; // Optional
    ShaderFileInfo cs; // Optional (Compute Shader)

    PipelineStateDesc baseDesc;
};

} // namespace Engine::Graphics
