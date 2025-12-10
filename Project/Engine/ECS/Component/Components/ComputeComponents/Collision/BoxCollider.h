#pragma once

/// engine
#include "ICollider.h"
#include "Engine/Core/Utility/Math/Vector3.h"


namespace ONEngine {

class BoxCollider;

namespace COMP_DEBUG {
/// @brief BoxColliderのデバッグ表示
/// @param _boxCollider 
void BoxColliderDebug(BoxCollider* _boxCollider);

}	/// namespace COMP_DEBUG

void from_json(const nlohmann::json& _j, BoxCollider& _b);
void to_json(nlohmann::json& _j, const BoxCollider& _b);

/// //////////////////////////////////////
/// BoxCollider
/// //////////////////////////////////////
class BoxCollider : public ICollider {
	/// --------------- friend function --------------- ///
	friend void COMP_DEBUG::BoxColliderDebug(BoxCollider* _boxCollider);
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

} /// ONEngine
