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

	AnimationRenderer();
	~AnimationRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================



};


class AnimationRendererCommon final {
public:

	void PreDraw();
	void PostDraw();

private:


};
