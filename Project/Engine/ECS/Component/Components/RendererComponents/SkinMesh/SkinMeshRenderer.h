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

	SkinMeshRenderer();
	~SkinMeshRenderer() override = default;

private:
	/// =========================================
	/// private : methods
	/// =========================================

	std::string meshPath_;
	std::string texturePath_;

	float animationTime_;
	float duration_;

public:
	/// ==========================================
	/// public : accessors
	/// ==========================================

	void SetMeshPath(const std::string& _path);
	void SetTexturePath(const std::string& _path);

	void SetAnimationTime(float _time);
	void SetDuration(float _duration);

	
	const std::string& GetMeshPath() const;
	const std::string& GetTexturePath() const;

	float GetAnimationTime() const;
	float GetDuration() const;

};


namespace COMP_DEBUG {
	void SkinMeshRendererDebug(SkinMeshRenderer* _smr);
}
