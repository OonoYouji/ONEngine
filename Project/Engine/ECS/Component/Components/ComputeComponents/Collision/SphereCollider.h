#pragma once

/// engine
#include "ICollider.h"


class SphereCollider;

namespace COMP_DEBUG {
	/// @brief SphereColliderのデバッグ表示
	/// @param _collider SphereColliderのポインタ
	void SphereColliderDebug(SphereCollider* _collider);
}

void from_json(const nlohmann::json& _j, SphereCollider& _c);
void to_json(nlohmann::json& _j, const SphereCollider& _c);


/// //////////////////////////////////////
/// SphereCollider
/// //////////////////////////////////////
class SphereCollider : public ICollider {
	friend void COMP_DEBUG::SphereColliderDebug(SphereCollider* _collider);
	friend void from_json(const nlohmann::json& _j, SphereCollider& _c);
	friend void to_json(nlohmann::json& _j, const SphereCollider& _c);
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
