#include "Shader.h"

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

void from_json(const nlohmann::json& j, ShaderStage& stage) {
    if(j.is_string()) {
        auto opt = magic_enum::enum_cast<ShaderStage>(
            j.get<std::string>(),
            magic_enum::case_insensitive
        );
        stage = opt.value_or(ShaderStage::Unkown);
    } else if(j.is_number()) {
        stage = static_cast<ShaderStage>(j.get<int>());
    } else {
        stage = ShaderStage::Unkown;
    }
}

void to_json(nlohmann::json& j, const ShaderStage& stage) {
    j = std::string(magic_enum::enum_name(stage));
}

Shader::Shader() = default;
Shader::~Shader() = default;

} /// namespace ONEngine::Asset