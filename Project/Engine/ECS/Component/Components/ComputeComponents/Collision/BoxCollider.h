#pragma once

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "ICollider.h"
#include "Engine/Core/Utility/Math/Vector3.h"


namespace ONEngine {

class BoxCollider;

namespace ComponentDebug {
/// @brief BoxColliderのデバッグ表示
/// @param _boxCollider 
void BoxColliderDebug(BoxCollider* _boxCollider);

}	/// namespace ComponentDebug

/// //////////////////////////////////////
/// BoxCollider
/// //////////////////////////////////////
class BoxCollider : public ICollider {
	/// --------------- friend function --------------- ///
	friend void ComponentDebug::BoxColliderDebug(BoxCollider* _boxCollider);
	friend void from_json(const nlohmann::json& _j, BoxCollider& _b);
	friend void to_json(nlohmann::json& _j, const BoxCollider& _b);
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	BoxCollider();
	~BoxCollider() override = default;

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	Vector3 size_;

public:
	/// =====================================================
	/// public : accessors
	/// =====================================================

	void SetSize(const Vector3& _size);
	const Vector3& GetSize() const;

};

/// @brief Mono Internal Calls
Vector3 InternalGetSize(uint64_t _nativeHandle);
void InternalSetSize(uint64_t _nativeHandle, Vector3 _size);
bool InternalIsTriggerBox(uint64_t _nativeHandle);
void InternalSetTriggerBox(uint64_t _nativeHandle, bool _trigger);
float InternalGetMassBox(uint64_t _nativeHandle);
void InternalSetMassBox(uint64_t _nativeHandle, float _mass);

} /// ONEngine
