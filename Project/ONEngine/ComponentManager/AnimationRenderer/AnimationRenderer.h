#pragma once

#include "../Base/BaseComponent.h"

/// ===================================================
/// アニメーションの
/// ===================================================
class AnimationRenderer final : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================



};
