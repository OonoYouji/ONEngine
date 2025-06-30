#pragma once

/// engine
#include "EntityComponentSystem.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// systems
#include "../System/MeshBufferRecreate/MeshBufferRecreate.h"
#include "../System/EffectUpdateSystem/EffectUpdateSystem.h"
#include "Game/Objects/Terrain/Collision/TerrainCollision.h"
#include "../System/ScriptUpdateSystem/ScriptUpdateSystem.h"
#include "../System/CollisionSystem/CollisionSystem.h"

inline void AddECSSystemFunction(EntityComponentSystem* _ecs, DxManager* _pDxManager) {
	_ecs->AddSystem<ScriptUpdateSystem>();
	_ecs->AddSystem<TerrainCollisionSystem>();
	_ecs->AddSystem<CollisionSystem>();
	_ecs->AddSystem<EffectUpdateSystem>();
	_ecs->AddSystem<MeshBufferRecreate>(_pDxManager->GetDxDevice());
}