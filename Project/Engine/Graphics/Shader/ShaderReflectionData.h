#pragma once

#include <string>
#include <vector>
#include <cstdint>

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
