#pragma once

/// engine
#include "EntityComponentSystem.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// systems
#include "../System/MeshBufferRecreate/MeshBufferRecreate.h"
#include "../System/EffectUpdateSystem/EffectUpdateSystem.h"
#include "Game/Objects/Terrain/Collision/TerrainCollision.h"
#include "../System/ScriptUpdateSystem/ScriptUpdateSystem.h"
#include "../System/Collision/CollisionSystem.h"
#include "../System/Collision/ColliderRenderQueueSystem.h"
#include "../System/SkinMesh/SkinMeshUpdateSystem.h"

inline void AddECSSystemFunction(EntityComponentSystem* _ecs, DxManager* _dxManager, class GraphicsResourceCollection* _resourceCollection) {
	_ecs->AddSystem<ScriptUpdateSystem>();
	_ecs->AddSystem<SkinMeshUpdateSystem>(_dxManager, _resourceCollection);
	_ecs->AddSystem<TerrainCollisionSystem>();
	_ecs->AddSystem<CollisionSystem>();
	_ecs->AddSystem<EffectUpdateSystem>();
	_ecs->AddSystem<MeshBufferRecreate>(_dxManager->GetDxDevice());
	_ecs->AddSystem<ColliderRenderQueueSystem>();
}