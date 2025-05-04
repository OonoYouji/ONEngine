#pragma once

/// engine
#include "../../../Interface/IComponent.h"

/// ///////////////////////////////////////////////////
/// 地形との当たり判定に使用するコンポーネント
/// ///////////////////////////////////////////////////
class ToTerrainCollider : public IComponent {
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


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	void SetIsCollided(bool _isCollided) { isCollided_ = _isCollided; }


	bool GetIsCollided() const { return isCollided_; }

};

