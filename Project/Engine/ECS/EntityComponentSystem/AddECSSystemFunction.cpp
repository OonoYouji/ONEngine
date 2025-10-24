#include "AddECSSystemFunction.h"

/// engine
#include "ECSGroup.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// systems
#include "../System/Audio/AudioPlaybackSystem.h"
#include "../System/MeshBufferRecreate/MeshBufferRecreate.h"
#include "../System/EffectUpdateSystem/EffectUpdateSystem.h"
#include "../System/ScriptUpdateSystem/ScriptUpdateSystem.h"
#include "../System/Sprite/SpriteUpdateSystem.h"
#include "../System/Collision/CollisionSystem.h"
#include "../System/Collision/ColliderRenderQueueSystem.h"
#include "../System/SkinMesh/SkinMeshUpdateSystem.h"
#include "../System/CameraUpdate/CameraUpdateSystem.h"
#include "../System/Terrain/TerrainColliderVertexGenerator.h"
#include "../System/Terrain/TerrainCollision.h"
#include "../System/Transform/TransformUpdateSystem.h"
#include "../System/GrassBufferCreateSystem/GrassBufferCreateSystem.h"

void GameECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxm, AssetCollection* _assetCollection) {

	/// 初期化に使うsystem
	_ecs->AddSystem<TerrainColliderVertexGenerator>(_dxm);
	_ecs->AddSystem<GrassBufferCreateSystem>(_dxm);

	/// 更新に使うsystem
	_ecs->AddSystem<CameraUpdateSystem>(_dxm->GetDxDevice());
	_ecs->AddSystem<SkinMeshUpdateSystem>(_dxm, _assetCollection);
	_ecs->AddSystem<ScriptUpdateSystem>(_ecs);
	_ecs->AddSystem<AudioPlaybackSystem>(_assetCollection);
	_ecs->AddSystem<EffectUpdateSystem>();
	_ecs->AddSystem<SpriteUpdateSystem>();
	_ecs->AddSystem<TransformUpdateSystem>();

	/// 衝突判定に使うsystem
	_ecs->AddSystem<TerrainCollision>();
	_ecs->AddSystem<CollisionSystem>();

	/// 描画に使うsystem
	_ecs->AddSystem<MeshBufferRecreate>(_dxm->GetDxDevice());
	_ecs->AddSystem<ColliderRenderQueueSystem>();
}

void DebugECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxm, AssetCollection* _assetCollection) {

	/// 初期化に使うsystem
	_ecs->AddSystem<TerrainColliderVertexGenerator>(_dxm);
	_ecs->AddSystem<GrassBufferCreateSystem>(_dxm);

	/// 更新に使うsystem
	_ecs->AddSystem<CameraUpdateSystem>(_dxm->GetDxDevice());
	_ecs->AddSystem<SkinMeshUpdateSystem>(_dxm, _assetCollection);
	_ecs->AddSystem<DebugScriptUpdateSystem>(_ecs);
	_ecs->AddSystem<AudioPlaybackSystem>(_assetCollection);
	_ecs->AddSystem<EffectUpdateSystem>();
	_ecs->AddSystem<SpriteUpdateSystem>();
	_ecs->AddSystem<TransformUpdateSystem>();

	/// 衝突判定に使うsystem
	_ecs->AddSystem<TerrainCollision>();
	_ecs->AddSystem<CollisionSystem>();

	/// 描画に使うsystem
	_ecs->AddSystem<MeshBufferRecreate>(_dxm->GetDxDevice());
	_ecs->AddSystem<ColliderRenderQueueSystem>();
}
