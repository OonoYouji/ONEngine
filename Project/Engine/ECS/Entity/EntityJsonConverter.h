#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

namespace EntityJsonConverter {
	nlohmann::json ToJson(const GameEntity* _entity);

	void FromJson(const nlohmann::json& _json, GameEntity* _entity, const std::string& _groupName);
	void TransformFromJson(const nlohmann::json& _json, GameEntity* _entity);
};

