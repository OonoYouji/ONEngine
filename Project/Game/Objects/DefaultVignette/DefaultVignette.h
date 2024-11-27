#pragma once

#include "GameObjectManager/BaseGameObject.h"

class DefaultVignette : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DefaultVignette();
	~DefaultVignette();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;

};
