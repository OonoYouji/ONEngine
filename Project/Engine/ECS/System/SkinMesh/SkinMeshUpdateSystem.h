#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"

/// /////////////////////////////////////////////
/// スキンメッシュの更新システム
/// /////////////////////////////////////////////
namespace ONEngine {

class SkinMeshUpdateSystem : public ECSISystem {
public:

	SkinMeshUpdateSystem(class DxManager* _dxm, class AssetCollection* _assetCollection);
	~SkinMeshUpdateSystem() override = default;

	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/// @brief スケルトンの更新
	void UpdateSkeleton(class SkinMeshRenderer* _smr);

	/// @brief スキンクラスターの更新
	void UpdateSkinCluster(class SkinMeshRenderer* _smr);

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class AssetCollection* pAssetCollection_; ///< グラフィックスリソースコレクション
	class DxManager* pDxManager_; ///< DirectXマネージャー

};


} /// ONEngine
