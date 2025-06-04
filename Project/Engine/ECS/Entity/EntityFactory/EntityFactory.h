#pragma once

/// std
#include <functional>
#include <unordered_map>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

//using GenerateFunc = std::function<IEntity* (void)>;
class IEntity;
using GenerateFunc = std::function<std::unique_ptr<IEntity>(void)>;

/// ///////////////////////////////////////////////////
/// EntityFactory
/// ///////////////////////////////////////////////////
class EntityFactory {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityFactory(class EntityComponentSystem* _ecs);
	~EntityFactory();

	void Register(const std::string& _entityName, GenerateFunc _generateFunc);

	std::unique_ptr<IEntity> Generate(const std::string& _entityName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class EntityComponentSystem* pECS_ = nullptr;

	std::unordered_map<std::string, GenerateFunc> factoryMap_;

};

