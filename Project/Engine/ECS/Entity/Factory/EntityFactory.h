#pragma once

/// std
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>

/// engine
#include "../Interface/IEntity.h"

using GenerateFunc = std::function<std::unique_ptr<IEntity>(void)>;

/// ///////////////////////////////////////////////////
/// EntityFactory
/// ///////////////////////////////////////////////////
class EntityFactory {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityFactory();
	~EntityFactory();

	void Register(const std::string& _entityName, GenerateFunc _generateFunc);

	std::unique_ptr<IEntity> Generate(const std::string& _entityName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	//class EntityComponentSystem* pECS_ = nullptr;

	std::unordered_map<std::string, GenerateFunc> factoryMap_;

};

