#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

namespace ONEngine {

namespace EntityJsonConverter {
nlohmann::json ToJson(const GameEntity* _entity);


/// @brief JsonからGameEntityを生成する
/// @param _json GameEntityのJsonデータ
/// @param _entity 生成に使用するGameEntityのポインタ
/// @param _groupName 生成元のECSGroup名
void FromJson(const nlohmann::json& _json, GameEntity* _entity, const std::string& _groupName);

/// @brief Transform専用のJsonからGameEntityを生成する
/// @param _json 生成元のJsonデータ
/// @param _entity 生成先のGameEntityのポインタ
void TransformFromJson(const nlohmann::json& _json, GameEntity* _entity);
};

} /// namespace ONEngine