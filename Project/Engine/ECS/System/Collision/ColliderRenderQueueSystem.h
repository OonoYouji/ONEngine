#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"

/// ///////////////////////////////////////////////////////
/// コライダーの可視化を行うシステム
/// ///////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ColliderRenderQueueSystem
 * @brief エディタでのシーン編集時などに、コライダー（Sphere/Box等）のデバッグ用可視化ギズモを描画キューに登録するシステムクラス
 */
class ColliderRenderQueueSystem : public ECSISystem {
public:
	/// =========================================================
	/// public : methods
	/// =========================================================

	/**
	 * @brief コンストラクタ
	 */
	ColliderRenderQueueSystem();

	/**
	 * @brief デストラクタ
	 */
	~ColliderRenderQueueSystem() override = default;

	/**
	 * @brief エディタ非実行時のコライダー可視化処理（ギズモの描画）を行います。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時の更新処理（可視化はエディタ時のみ行うため、処理は空です）。
	 */
	void RuntimeUpdate(class ECSGroup*) override {};

	/**
	 * @brief 球体コライダー（SphereCollider）の情報からデバッグ用の球体ギズモを描画キューへ追加します。
	 * @param _sphereColliderArray 球体コライダーのコンポーネント配列へのポインタ
	 */
	void UpdateSphereCollider(ComponentArray<SphereCollider>* _sphereColliderArray);

	/**
	 * @brief ボックスコライダー（BoxCollider）の情報からデバッグ用の直方体ギズモを描画キューへ追加します。
	 * @param _boxColliderArray ボックスコライダーのコンポーネント配列へのポインタ
	 */
	void UpdateBoxCollider(ComponentArray<BoxCollider>* _boxColliderArray);

};


} /// ONEngine
