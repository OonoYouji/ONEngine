#pragma once

#include "EntityFactory.h"
/// entities
#include "../Entities/Camera/DebugCamera.h"
#include "../Entities/EmptyEntity/EmptyEntity.h"



inline void AddEntityFactory(EntityFactory* _factory, class DxDevice* _dxDevice) {
	_factory->Register("DebugCamera", [_dxDevice]() { return std::make_unique<DebugCamera>(); });
	_factory->Register("EmptyEntity", []() { return std::make_unique<EmptyEntity>(); });
}

