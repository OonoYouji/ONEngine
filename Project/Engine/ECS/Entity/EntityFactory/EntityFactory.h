#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


class EntityFactory {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityFactory();
	~EntityFactory();

	template<typename T>
	void Register() requires std::is_base_of_v<IEntity, T>;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

};

