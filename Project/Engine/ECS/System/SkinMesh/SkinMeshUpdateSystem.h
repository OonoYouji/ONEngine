#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"

namespace ONEngine {
class DxManager;
class ECSGroup;
class SkinMeshRenderer;
}

namespace ONEngine::Asset {
class AssetCollection;
}


/// /////////////////////////////////////////////
/// スキンメッシュの更新システム
/// /////////////////////////////////////////////
namespace ONEngine {

class SkinMeshUpdateSystem : public ECSISystem {
public:

	SkinMeshUpdateSystem(DxManager* _dxm, Asset::AssetCollection* _assetCollection);
	~SkinMeshUpdateSystem() override = default;

	void RuntimeUpdate(ECSGroup* _ecs) override;

	/// @brief スケルトンの更新
	void UpdateSkeleton(SkinMeshRenderer* _smr);

	/// @brief スキンクラスターの更新
	void UpdateSkinCluster(SkinMeshRenderer* _smr);

private:
	/// @brief 再帰的にスケルトンを更新
	void UpdateSkeletonRecursive(SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex);

	/// =========================================
	/// private : objects
	/// =========================================

	Asset::AssetCollection* pAssetCollection_; ///< グラフィックスリソースコレクション
	DxManager* pDxManager_; ///< DirectXマネージャー

};


} /// ONEngine
