#include "GrassBufferCreateSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

GrassBufferCreateSystem::GrassBufferCreateSystem(DxManager* _dxm) : pDxManager_(_dxm) {}
GrassBufferCreateSystem::~GrassBufferCreateSystem() = default;

void GrassBufferCreateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	/// ----- ランタイム外で生成する(デバッグ時にRuntimeを起動していなくてもよいように) ----- ///

	/// -------------------------------------------------
	/// 早期リターン条件のチェック
	/// -------------------------------------------------

	ComponentArray<GrassField>* grassArray = _ecs->GetComponentArray<GrassField>();
	if (!grassArray) {
		return;
	}

	if (grassArray->GetUsedComponents().empty()) {
		return;
	}


	/// -------------------------------------------------
	/// 実際に処理する
	/// -------------------------------------------------

	/// 縦x横 で密度を決める (1500個 x 1500個 = 225万本)
	uint32_t maxBladeCount = 1500 * 1500;
	for (auto& grass : grassArray->GetUsedComponents()) {
		grass->Initialize(
			maxBladeCount,
			pDxManager_->GetDxDevice(),
			pDxManager_->GetDxCommand(),
			pDxManager_->GetDxSRVHeap()
		);
	}
}

void GrassBufferCreateSystem::RuntimeUpdate(ECSGroup* /*_ecs*/) {
	// ランタイム中の更新処理をここに実装
}

