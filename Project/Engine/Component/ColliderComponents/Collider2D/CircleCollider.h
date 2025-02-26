#pragma once

/// engine
#include "../../IComponent.h"

/// ===================================================
/// 2dの円の当たり判定
/// ===================================================
class CircleCollider : public IComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	CircleCollider();
	~CircleCollider();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float radius_ = 0.0f;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief 半径のゲット
	/// @return 半径
	float GetRadius() const { return radius_; }


	/// @brief 半径のセット
	/// @param _radius 半径
	void SetRadius(float _radius) { radius_ = _radius; }

};

