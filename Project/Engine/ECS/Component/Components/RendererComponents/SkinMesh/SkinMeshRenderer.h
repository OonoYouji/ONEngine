#pragma once

/// std
#include <string>

/// engine
#include "../../Interface/IComponent.h"

/// /////////////////////////////////////////////
/// スキニングアニメーションのRender Component
/// /////////////////////////////////////////////
class SkinMeshRenderer : public IRenderComponent {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	SkinMeshRenderer() = default;
	~SkinMeshRenderer() override = default;

private:
	/// =========================================
	/// private : methods
	/// =========================================

	std::string meshPath_;
	std::string texturePath_;

	//float animationSpeed_ = 1.0f; ///< アニメーションの速度
	//float animationTime_ = 0.0f; ///< アニメーションの時間
	//float duration_;

};


namespace DEBUG_COMP {
	void SkinMeshRendererDebug(SkinMeshRenderer* _smr);
}
