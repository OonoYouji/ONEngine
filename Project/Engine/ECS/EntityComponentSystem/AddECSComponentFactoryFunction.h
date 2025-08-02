#pragma once

#include "EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Collection/ComponentCollection.h"

inline void AddComponentFactoryFunction(ComponentCollection* _compCollection) {
	/// compute
	_compCollection->RegisterComponentFactory<Transform>();
	_compCollection->RegisterComponentFactory<DirectionalLight>();
	_compCollection->RegisterComponentFactory<AudioSource>();
	_compCollection->RegisterComponentFactory<Variables>();
	_compCollection->RegisterComponentFactory<Effect>();
	_compCollection->RegisterComponentFactory<Terrain>();

	/// renderer
	_compCollection->RegisterComponentFactory<MeshRenderer>();
	_compCollection->RegisterComponentFactory<CustomMeshRenderer>();
	_compCollection->RegisterComponentFactory<SkinMeshRenderer>();
	_compCollection->RegisterComponentFactory<SpriteRenderer>();
	_compCollection->RegisterComponentFactory<Line2DRenderer>();
	_compCollection->RegisterComponentFactory<Line3DRenderer>();
	_compCollection->RegisterComponentFactory<ScreenPostEffectTag>();
	_compCollection->RegisterComponentFactory<Skybox>();

	/// collider
	_compCollection->RegisterComponentFactory<ToTerrainCollider>();
	_compCollection->RegisterComponentFactory<SphereCollider>();
	_compCollection->RegisterComponentFactory<BoxCollider>();
}
