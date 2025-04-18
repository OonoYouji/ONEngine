#pragma once

/// engine
#include "EntityComponentSystem.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// systems
#include "../System/MeshBufferRecreate/MeshBufferRecreate.h"
#include "../System/EffectUpdateSystem/EffectUpdateSystem.h"

inline void AddECSSystemFunction(EntityComponentSystem* _ecs, DxManager* _pDxManager) {
	_ecs->AddSystem<MeshBufferRecreate>(_pDxManager->GetDxDevice());
	_ecs->AddSystem<EffectUpdateSystem>();
}