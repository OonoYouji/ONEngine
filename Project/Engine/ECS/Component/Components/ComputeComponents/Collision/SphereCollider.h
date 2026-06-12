#pragma once

/// engine
#include "ICollider.h"


namespace ONEngine {

class SphereCollider;

namespace ComponentDebug {
/// @brief SphereColliderのデバッグ表示
/// @param _collider SphereColliderのポインタ
void SphereColliderDebug(SphereCollider* _collider);
}

/// //////////////////////////////////////
/// SphereCollider
/// //////////////////////////////////////
class SphereCollider : public ICollider {
	friend void ComponentDebug::SphereColliderDebug(SphereCollider* _collider);
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

/// @brief Mono Internal Calls
float InternalGetRadius(uint64_t _nativeHandle);
void InternalSetRadius(uint64_t _nativeHandle, float _radius);
bool InternalIsTriggerSphere(uint64_t _nativeHandle);
void InternalSetTriggerSphere(uint64_t _nativeHandle, bool _trigger);
float InternalGetMass(uint64_t _nativeHandle);
void InternalSetMass(uint64_t _nativeHandle, float _mass);

} /// ONEngine
