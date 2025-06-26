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

