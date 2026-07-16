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

/**
 * @class SkinMeshUpdateSystem
 * @brief スキンメッシュ（SkinMeshRenderer）のアニメーションに基づくボーン階層行列計算（スケルトン更新）や、GPU転送用スキンクラスターバッファの更新処理を司るシステムクラス
 */
class SkinMeshUpdateSystem : public ECSISystem {
public:

	/**
	 * @brief コンストラクタ
	 * @param _dxm GPUリソース管理用の DirectX12 マネージャー
	 * @param _assetCollection モデルやアニメーションアセット取得用のコレクション
	 */
	SkinMeshUpdateSystem(DxManager* _dxm, Asset::AssetCollection* _assetCollection);

	/**
	 * @brief デストラクタ
	 */
	~SkinMeshUpdateSystem() override = default;

	/**
	 * @brief 毎フレームのスキンメッシュ更新処理を実行します（アニメーションがAnimator経由でないレガシー更新等も含む）。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void RuntimeUpdate(ECSGroup* _ecs) override;

	/**
	 * @brief スキンメッシュ内の全ジョイント（関節）の姿勢から、各ボーンのワールド空間行列（スケルトン行列）を計算し更新します。
	 * @param _smr 対象となるスキンメッシュレンダラー
	 */
	void UpdateSkeleton(SkinMeshRenderer* _smr);

	/**
	 * @brief 各ジョイントの計算結果から、GPUスキンクラスター用のパレットマトリクスバッファを同期・更新します。
	 * @param _smr 対象となるスキンメッシュレンダラー
	 */
	void UpdateSkinCluster(SkinMeshRenderer* _smr);

private:
	/**
	 * @brief ボーン階層構造の親子関係に従い、再帰的にワールド空間のジョイント行列を更新します。
	 * @param _smr 対象となるスキンメッシュレンラー
	 * @param _jointIndex 処理する現在のジョイントインデックス
	 * @param _parentIndex 親ジョイントのインデックス
	 */
	void UpdateSkeletonRecursive(SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex);

	/// =========================================
	/// private : objects
	/// =========================================

	Asset::AssetCollection* pAssetCollection_; ///< グラフィックスリソースコレクション
	DxManager* pDxManager_; ///< DirectXマネージャー

};


} /// ONEngine
