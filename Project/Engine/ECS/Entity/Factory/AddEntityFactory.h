#pragma once

#include "EntityFactory.h"
/// entities
#include "../Entities/Camera/DebugCamera.h"
#include "../Entities/Demo/DemoEntity.h"
#include "../Entities/EmptyEntity/EmptyEntity.h"
#include "../Entities/Grid/Grid.h"
#include "../Entities/Light/DirectionalLightObject.h"



inline void AddEntityFactory(EntityFactory* _factory, class DxDevice* _dxDevice) {
	_factory->Register("DebugCamera", [_dxDevice]() { return std::make_unique<DebugCamera>(); });
	_factory->Register("DemoEntity", []() { return std::make_unique<DemoEntity>(); });
	_factory->Register("EmptyEntity", []() { return std::make_unique<EmptyEntity>(); });
	_factory->Register("Grid", []() { return std::make_unique<Grid>(); });
	_factory->Register("DirectionalLightObject", []() { return std::make_unique<DirectionalLightObject>(); });
}

