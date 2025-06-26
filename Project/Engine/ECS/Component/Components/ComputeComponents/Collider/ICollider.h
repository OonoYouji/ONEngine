#pragma once

/// std
#include <functional>

/// engine
#include "../../Interface/IComponent.h"

using ColliderCallBackFunc = std::function<void(class ICollider*)>;

class ICollider : public IComponent {
	friend class CollisionManager;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ICollider() = default;
	~ICollider() override = default;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ColliderCallBackFunc onCollisionEnterCallBackFunc_;
	ColliderCallBackFunc onCollisionExitCallBackFunc_;
	ColliderCallBackFunc onCollisionStayCallBackFunc_;


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	void SetOnCollisionEnterCallBackFunc(ColliderCallBackFunc _func);
	void SetOnCollisionExitCallBackFunc(ColliderCallBackFunc _func);
	void SetOnCollisionStayCallBackFunc(ColliderCallBackFunc _func);


};

