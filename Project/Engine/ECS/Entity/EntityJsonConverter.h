#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

namespace EntityJsonConverter {
	nlohmann::json ToJson(const IEntity* _entity);

	void FromJson(const nlohmann::json& _json, IEntity* _entity);
};

