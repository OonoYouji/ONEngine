#pragma once

#include "EntityComponentSystem.h"
#include "Engine/ECS/Component/Collection/ComponentCollection.h"

/// compute
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"

/// renderer
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"

inline void AddComponentFactoryFunction(ComponentCollection* _compCollection) {
	/// compute
	_compCollection->RegisterComponentFactory<Transform>();
	_compCollection->RegisterComponentFactory<DirectionalLight>();
	_compCollection->RegisterComponentFactory<AudioSource>();
	_compCollection->RegisterComponentFactory<Variables>();
	_compCollection->RegisterComponentFactory<Effect>();
	_compCollection->RegisterComponentFactory<Terrain>();
	_compCollection->RegisterComponentFactory<GrassField>();
	_compCollection->RegisterComponentFactory<TerrainCollider>();
	_compCollection->RegisterComponentFactory<CameraComponent>();
	_compCollection->RegisterComponentFactory<ShadowCaster>();
	_compCollection->RegisterComponentFactory<VoxelTerrain>();

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
	_compCollection->RegisterComponentFactory<SphereCollider>();
	_compCollection->RegisterComponentFactory<BoxCollider>();
}
