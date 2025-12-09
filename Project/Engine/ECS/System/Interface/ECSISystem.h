#pragma once

/// std
#include <vector>

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
namespace ONEngine {

class ECSISystem {
public:

	virtual ~ECSISystem() {}

	virtual void OutsideOfRuntimeUpdate(class ECSGroup* /*_ecs*/) {}
	virtual void RuntimeUpdate(class ECSGroup* _ecs) = 0;

};



} /// ONEngine
