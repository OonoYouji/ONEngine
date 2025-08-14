#pragma once

/// std
#include <vector>

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
class ECSISystem {
public:

	virtual ~ECSISystem() {}

	virtual void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs);
	virtual void RuntimeUpdate(class EntityComponentSystem* _ecs) = 0;

};


