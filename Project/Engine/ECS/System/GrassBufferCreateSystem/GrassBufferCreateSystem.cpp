#include "GrassBufferCreateSystem.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

GrassBufferCreateSystem::GrassBufferCreateSystem(DxManager* _dxm) : pDxManager_(_dxm) {}
GrassBufferCreateSystem::~GrassBufferCreateSystem() = default;

void GrassBufferCreateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	/// ランタイム外で生成する

	/// =================================================
	/// 早期リターン条件のチェック
	/// =================================================

	ComponentArray<GrassField>* grassArray = _ecs->GetComponentArray<GrassField>();
	if (!grassArray) {
		return;
	}

	if (grassArray->GetUsedComponents().empty()) {
		return;
	}


	/// =================================================
	/// 実際に処理する
	/// =================================================

	uint32_t maxBladeCount = 1000 * 1000;
	//uint32_t maxBladeCount = static_cast<uint32_t>(std::pow(2, 32) - 1);
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

