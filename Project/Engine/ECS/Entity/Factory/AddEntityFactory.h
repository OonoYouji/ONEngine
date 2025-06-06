#pragma once

#include "EntityFactory.h"
/// entities
#include "../Entities/Camera/Camera.h"
#include "../Entities/Camera/DebugCamera.h"
#include "../Entities/Demo/DemoEntity.h"
#include "../Entities/EmptyEntity/EmptyEntity.h"
#include "../Entities/Grid/Grid.h"
#include "../Entities/Light/DirectionalLightObject.h"
#include "../Entities/Skybox/Skybox.h"

/*
using GenerateFunc = std::function<std::unique_ptr<IEntity>(void)>;
*/


inline void AddEntityFactory(EntityFactory* _factory, class DxDevice* _dxDevice) {
	_factory->Register("Camera", [_dxDevice]() { return std::make_unique<Camera>(_dxDevice); });
	_factory->Register("DebugCamera", [_dxDevice]() { return std::make_unique<DebugCamera>(_dxDevice); });
	_factory->Register("DemoEntity", []() { return std::make_unique<DemoEntity>(); });
	_factory->Register("EmptyEntity", []() { return std::make_unique<EmptyEntity>(); });
	_factory->Register("Grid", []() { return std::make_unique<Grid>(); });
	_factory->Register("DirectionalLightObject", []() { return std::make_unique<DirectionalLightObject>(); });
	_factory->Register("Skybox", []() { return std::make_unique<Skybox>();  });
}

