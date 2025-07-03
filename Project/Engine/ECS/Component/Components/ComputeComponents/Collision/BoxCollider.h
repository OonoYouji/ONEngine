#pragma once

/// engine
#include "ICollider.h"
#include "Engine/Core/Utility/Math/Vector3.h"


/// //////////////////////////////////////
/// BoxCollider
/// //////////////////////////////////////
class BoxCollider : public ICollider {
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


namespace COMP_DEBUG {
	/// @brief BoxColliderのデバッグ表示
	/// @param _boxCollider 
	void BoxColliderDebug(BoxCollider* _boxCollider);

}	/// namespace COMP_DEBUG