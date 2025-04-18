#pragma once

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
class ECSISystem {
public:

	virtual ~ECSISystem() {}

	virtual void Update(class EntityComponentSystem* _pEntityComponentSystem) = 0;
};

