#pragma once

/// std
#include <functional>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"

using ColliderCallBackFunc = std::function<void(class ICollider*)>;


/// ///////////////////////////////////////////////////
/// Colliderのインターフェース
/// ///////////////////////////////////////////////////
class ICollider : public IComponent {
	friend class CollisionManager;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ICollider() = default;
	~ICollider() override = default;

	/// @brief 1frame前の座標を更新する
	void UpdatePrevPosition();

	const Vector3& GetPrevPosition() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ColliderCallBackFunc onCollisionEnterCallBackFunc_;
	ColliderCallBackFunc onCollisionExitCallBackFunc_;
	ColliderCallBackFunc onCollisionStayCallBackFunc_;

	Vector3 prevPosition_;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	void SetOnCollisionEnterCallBackFunc(ColliderCallBackFunc _func);
	void SetOnCollisionExitCallBackFunc(ColliderCallBackFunc _func);
	void SetOnCollisionStayCallBackFunc(ColliderCallBackFunc _func);


};

