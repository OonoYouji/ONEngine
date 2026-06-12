#pragma once

/// std
#include <functional>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "CollisionFilter.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// 衝突状態の列挙型
/// ///////////////////////////////////////////////////
enum class CollisionState {
	Static,
	Dynamic,
};

/// ///////////////////////////////////////////////////
/// Colliderのインターフェース
/// ///////////////////////////////////////////////////
class ICollider : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ICollider() = default;
	~ICollider() override = default;

	/// @brief 1frame前の座標を更新する
	void UpdatePrevPosition();

	/// @brief 前フレームの位置を返す
	/// @return 前フレームの位置(ワールド座標)
	const Vector3& GetPrevPosition() const;

	/// @brief コライダーの状態(静的、動的)かを返す、これを用いて押し戻しをするかどうかを判定する
	/// @return コライダーの状態
	CollisionState GetCollisionState() const;


	/// @brief 自身の衝突レイヤーを返す、これを用いてどのレイヤーと当たり判定するかを判定する
	/// @return 衝突レイヤーのビット
	uint32_t GetCategoryBits() const {
		return categoryBits_;
	}

	/// @brief 当たり判定のマスクビットを返す、これを用いてどのレイヤーと当たり判定するかを判定する
	/// @return 当たり判定のマスクビット
	uint32_t GetMaskBits() const {
		return maskBits_;
	}

	/// @brief 衝突フィルタのビットを設定する
	/// @param categoryBits 衝突レイヤーのビット
	/// @param maskBits 当たり判定のマスクビット
	void SetFilterBits(uint32_t categoryBits, uint32_t maskBits) {
		categoryBits_ = categoryBits;
		maskBits_ = maskBits;
	}

	/// @brief 自身の衝突レイヤーのビットを設定する
	/// @param categoryBits 衝突レイヤーのビット
	void SetCategoryBits(uint32_t categoryBits) {
		categoryBits_ = categoryBits;
	}

	/// @brief 自身の当たり判定のマスクビットを設定する
	/// @param maskBits 当たり判定のマスクビット
	void SetMaskBits(uint32_t maskBits) {
		maskBits_ = maskBits;
	}

	/// @brief Y軸の押し戻しを固定するかどうかを返す
	/// @return Y軸の押し戻しを固定するかどうか
	bool IsFreezeY() const {
		return freezeY_;
	}

	/// @brief Y軸の押し戻しを固定するかどうかを設定する
	/// @param _freeze Y軸の押し戻しを固定するかどうか
	void SetFreezeY(bool _freeze) {
		freezeY_ = _freeze;
	}

	/// @brief 質量を返す
	/// @return 質量
	float GetMass() const {
		return mass_;
	}

	/// @brief 質量を設定する
	/// @param _mass 質量
	void SetMass(float _mass) {
		mass_ = _mass;
	}


	/// @brief 押し戻しを発生させない「トリガー」モードかどうかを返す
	/// @return トリガーモードかどうか
	bool IsTrigger() const {
		return isTrigger_;
	}

	/// @brief トリガーモードを設定する（trueなら押し戻しが発生しなくなる）
	/// @param _trigger トリガーモードにするか
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
