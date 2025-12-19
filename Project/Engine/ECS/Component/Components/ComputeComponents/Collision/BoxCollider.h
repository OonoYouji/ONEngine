#pragma once

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "ICollider.h"
#include "Engine/Core/Utility/Math/Vector3.h"


namespace ONEngine {

class BoxCollider;

#ifdef DEBUG_MODE
namespace ComponentDebug {
/// @brief BoxColliderのデバッグ表示
/// @param _boxCollider 
void BoxColliderDebug(BoxCollider* _boxCollider);

}	/// namespace ComponentDebug
#endif

void from_json(const nlohmann::json& _j, BoxCollider& _b);
void to_json(nlohmann::json& _j, const BoxCollider& _b);

/// //////////////////////////////////////
/// BoxCollider
/// //////////////////////////////////////
class BoxCollider : public ICollider {
	/// --------------- friend function --------------- ///
#ifdef DEBUG_MODE
	friend void ComponentDebug::BoxColliderDebug(BoxCollider* _boxCollider);
#endif // DEBUG_MODE
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

#ifdef DEBUG_MODE 
	bool dUnified_ = false;
#endif 

public:
	/// =====================================================
	/// public : accessors
	/// =====================================================

	void SetSize(const Vector3& _size);
	const Vector3& GetSize() const;

};

} /// ONEngine
