#pragma once

/// engine
#include "../../../Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// 地形との当たり判定に使用するコンポーネント
/// ///////////////////////////////////////////////////
class ToTerrainCollider : public IComponent {
	friend class TerrainCollisionSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ToTerrainCollider() = default;
	~ToTerrainCollider() = default;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isCollided_ = false; ///< 地形と衝突しているか

	Vector3 prevPosition_;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	void SetIsCollided(bool _isCollided);

	bool GetIsCollided() const;

	const Vector3& GetPrevPosition() const;
};

