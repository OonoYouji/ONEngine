#pragma once

/// engine
#include "ICollider.h"


/// //////////////////////////////////////
/// SphereCollider
/// //////////////////////////////////////
class SphereCollider : public ICollider {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	SphereCollider();
	~SphereCollider() override = default;

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	float radius_;

public:
	/// =====================================================
	/// public : accessors
	/// =====================================================

	void SetRadius(float _radius);
	float GetRadius() const;

};


namespace COMP_DEBUG {
	/// @brief SphereColliderのデバッグ表示
	/// @param _collider SphereColliderのポインタ
	void SphereColliderDebug(SphereCollider* _collider);
}