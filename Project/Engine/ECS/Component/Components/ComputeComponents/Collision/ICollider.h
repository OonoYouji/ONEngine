#pragma once

/// std
#include <functional>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"


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

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	Vector3 prevPosition_;
	bool enablePushBack_ = true;
	CollisionState collisionState_ = CollisionState::Dynamic;

};

