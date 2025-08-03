#pragma once

/// std
#include <vector>

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
class ECSISystem {
public:

	virtual ~ECSISystem() {}

	virtual void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities);
	virtual void RuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) = 0;

};


