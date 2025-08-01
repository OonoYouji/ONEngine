#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"

/// /////////////////////////////////////////////
/// スキンメッシュの更新システム
/// /////////////////////////////////////////////
class SkinMeshUpdateSystem : public ECSISystem {
public:

	SkinMeshUpdateSystem(class DxManager* _dxManager, class GraphicsResourceCollection* _resourceCollection);
	~SkinMeshUpdateSystem() override = default;

	void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

	void UpdateSkeleton(class SkinMeshRenderer* _smr);
	void UpdateSkinCluster(class SkinMeshRenderer* _smr);

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class GraphicsResourceCollection* pResourceCollection_; ///< グラフィックスリソースコレクション
	class DxManager* pDxManager_; ///< DirectXマネージャー

};

