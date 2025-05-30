#pragma once

#include "EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

inline void AddComponentFactoryFunction(EntityComponentSystem* _ecs) {
	_ecs->RegisterComponentFactory<Transform>();
	_ecs->RegisterComponentFactory<DirectionalLight>();
	_ecs->RegisterComponentFactory<AudioSource>();
	_ecs->RegisterComponentFactory<Variables>();
	_ecs->RegisterComponentFactory<Effect>();

	_ecs->RegisterComponentFactory<MeshRenderer>();
	_ecs->RegisterComponentFactory<CustomMeshRenderer>();
	_ecs->RegisterComponentFactory<SpriteRenderer>();
	_ecs->RegisterComponentFactory<Line2DRenderer>();
	_ecs->RegisterComponentFactory<Line3DRenderer>();

	_ecs->RegisterComponentFactory<ToTerrainCollider>();
}
