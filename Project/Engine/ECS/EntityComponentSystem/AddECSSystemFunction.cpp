#include "AddECSSystemFunction.h"

/// engine
#include "ECSGroup.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// systems
#include "../System/MeshBufferRecreate/MeshBufferRecreate.h"
#include "../System/EffectUpdateSystem/EffectUpdateSystem.h"
#include "../System/ScriptUpdateSystem/ScriptUpdateSystem.h"
#include "../System/Collision/CollisionSystem.h"
#include "../System/Collision/ColliderRenderQueueSystem.h"
#include "../System/SkinMesh/SkinMeshUpdateSystem.h"
#include "../System/CameraUpdate/CameraUpdateSystem.h"
#include "../System/Terrain/TerrainColliderVertexGenerator.h"
#include "../System/Terrain/TerrainCollision.h"

void GameECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection) {

	/// 初期化に使うsystem
	_ecs->AddSystem<TerrainColliderVertexGenerator>(_dxManager);

	/// 更新に使うsystem
	_ecs->AddSystem<CameraUpdateSystem>(_dxManager->GetDxDevice());
	_ecs->AddSystem<SkinMeshUpdateSystem>(_dxManager, _resourceCollection);
	_ecs->AddSystem<ScriptUpdateSystem>();
	_ecs->AddSystem<EffectUpdateSystem>();

	/// 衝突判定に使うsystem
	_ecs->AddSystem<TerrainCollision>();
	_ecs->AddSystem<CollisionSystem>();

	/// 描画に使うsystem
	_ecs->AddSystem<MeshBufferRecreate>(_dxManager->GetDxDevice());
	_ecs->AddSystem<ColliderRenderQueueSystem>();
}

void DebugECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection) {

	/// 初期化に使うsystem
	_ecs->AddSystem<TerrainColliderVertexGenerator>(_dxManager);

	/// 更新に使うsystem
	_ecs->AddSystem<CameraUpdateSystem>(_dxManager->GetDxDevice());
	_ecs->AddSystem<SkinMeshUpdateSystem>(_dxManager, _resourceCollection);
	_ecs->AddSystem<ScriptUpdateSystem>();
	_ecs->AddSystem<EffectUpdateSystem>();

	/// 衝突判定に使うsystem
	_ecs->AddSystem<TerrainCollision>();
	_ecs->AddSystem<CollisionSystem>();

	/// 描画に使うsystem
	_ecs->AddSystem<MeshBufferRecreate>(_dxManager->GetDxDevice());
	_ecs->AddSystem<ColliderRenderQueueSystem>();
}
