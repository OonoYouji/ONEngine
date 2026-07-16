#include "Shader.h"

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

/**
 * @brief jsonオブジェクトからShaderStage列挙型へのデシリアライズを行います。
 * @param j jsonオブジェクト
 * @param stage 変換先ShaderStageの参照
 */
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

/**
 * @brief ShaderStage列挙型からjsonオブジェクトへのシリアライズを行います。
 * @param j jsonオブジェクト
 * @param stage 変換元ShaderStage
 */
void to_json(nlohmann::json& j, const ShaderStage& stage) {
    j = std::string(magic_enum::enum_name(stage));
}

/**
 * @brief デフォルトコンストラクタ
 */
Shader::Shader() = default;

/**
 * @brief デストラクタ
 */
Shader::~Shader() = default;

} /// namespace ONEngine::Asset