#pragma once

/// std
#include <functional>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "CollisionFilter.h"

namespace ONEngine {

/**
 * @enum CollisionState
 * @brief 押し戻し挙動などを判定するためのコライダーの動的/静的状態定義
 */
enum class CollisionState {
	Static,  ///< 静的オブジェクト（押し戻されない）
	Dynamic, ///< 動的オブジェクト（押し戻される）
};

/**
 * @class ICollider
 * @brief 各種コライダーコンポーネント（Box, Sphere等）の共通基盤となる抽象インターフェースクラス
 */
class ICollider : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ICollider() = default;

	/**
	 * @brief デストラクタ
	 */
	~ICollider() override = default;

	/**
	 * @brief 前フレーム位置（prevPosition_）を現在の位置で更新します。
	 */
	void UpdatePrevPosition();

	/**
	 * @brief 前フレームのワールド座標位置を取得します。
	 */
	const Vector3& GetPrevPosition() const;

	/**
	 * @brief コライダーの動的/静的状態（CollisionState）を取得します。
	 */
	CollisionState GetCollisionState() const;


	/**
	 * @brief 自身の衝突フィルタカテゴリビット（自分がどのグループに属するか）を取得します。
	 */
	uint32_t GetCategoryBits() const {
		return categoryBits_;
	}

	/**
	 * @brief 自身の衝突フィルタマスクビット（どのグループと衝突するか）を取得します。
	 */
	uint32_t GetMaskBits() const {
		return maskBits_;
	}

	/**
	 * @brief 自身のカテゴリビットとマスクビットを一括設定します。
	 * @param categoryBits カテゴリビット
	 * @param maskBits マスクビット
	 */
	void SetFilterBits(uint32_t categoryBits, uint32_t maskBits) {
		categoryBits_ = categoryBits;
		maskBits_ = maskBits;
	}

	/**
	 * @brief 自身の衝突フィルタカテゴリビットを設定します。
	 */
	void SetCategoryBits(uint32_t categoryBits) {
		categoryBits_ = categoryBits;
	}

	/**
	 * @brief 自身の衝突フィルタマスクビットを設定します。
	 */
	void SetMaskBits(uint32_t maskBits) {
		maskBits_ = maskBits;
	}

	/**
	 * @brief 衝突時の押し戻し計算において、Y軸方向の移動を固定（Freeze）するかを取得します。
	 */
	bool IsFreezeY() const {
		return freezeY_;
	}

	/**
	 * @brief Y軸方向の押し戻し固定設定（trueで固定）を行います。
	 */
	void SetFreezeY(bool _freeze) {
		freezeY_ = _freeze;
	}

	/**
	 * @brief オブジェクトの質量（押し戻し比率の計算に使用）を取得します。
	 */
	float GetMass() const {
		return mass_;
	}

	/**
	 * @brief オブジェクトの質量を設定します。
	 */
	void SetMass(float _mass) {
		mass_ = _mass;
	}


	/**
	 * @brief 物理的な押し戻しが発生しない「トリガー（センサー）」モードかを判定します。
	 */
	bool IsTrigger() const {
		return isTrigger_;
	}

	/**
	 * @brief トリガーモードを設定します（trueで押し戻し無効・接触検知のみ）。
	 */
	void SetTrigger(bool _trigger) {
		isTrigger_ = _trigger;
	}


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	Vector3 prevPosition_;
	bool isTrigger_ = false;
	bool freezeY_ = false;
	float mass_ = 1.0f;
	CollisionState collisionState_ = CollisionState::Dynamic;

	uint32_t categoryBits_ = static_cast<uint32_t>(CollisionFilter::Default);
	uint32_t maskBits_ = static_cast<uint32_t>(CollisionFilter::ALL);

};


} /// ONEngine
