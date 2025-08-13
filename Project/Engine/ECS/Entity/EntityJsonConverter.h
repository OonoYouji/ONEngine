#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

namespace EntityJsonConverter {
	nlohmann::json ToJson(const GameEntity* _entity);

	void FromJson(const nlohmann::json& _json, GameEntity* _entity);
	void TransformFromJson(const nlohmann::json& _json, GameEntity* _entity);
};

