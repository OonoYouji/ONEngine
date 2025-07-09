#pragma once

/// std
#include <vector>

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
class ECSISystem {
public:

	virtual ~ECSISystem() {}

	virtual void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) = 0;

};


