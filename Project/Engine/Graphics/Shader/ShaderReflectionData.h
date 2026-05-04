#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>

namespace Engine::Graphics {

///
/// 定数バッファ内の変数情報
///
struct ShaderVariableInfo {
    std::string name;
    uint32_t offset;
    uint32_t size;
};

///
/// 定数バッファ情報
///
struct ShaderConstantBufferInfo {
    std::string name;
    uint32_t bindPoint;
    uint32_t bindCount;
    uint32_t space;
    uint32_t size;
    D3D_SHADER_INPUT_TYPE type; // 追加: D3D_SIT_CBUFFER固定
    std::vector<ShaderVariableInfo> variables;
};

///
/// リソース情報（テクスチャ、バッファ等）
///
struct ShaderResourceInfo {
    std::string name;
    uint32_t bindPoint;
    uint32_t bindCount;
    uint32_t space;
    D3D_SHADER_INPUT_TYPE type; // 追加
};

///
/// シェーダーの反射情報
///
struct ShaderReflectionData {
    std::vector<ShaderConstantBufferInfo> constantBuffers;
    std::vector<ShaderResourceInfo> srvs;
    std::vector<ShaderResourceInfo> uavs;
    std::vector<ShaderResourceInfo> samplers;
};

} // namespace Engine::Graphics
