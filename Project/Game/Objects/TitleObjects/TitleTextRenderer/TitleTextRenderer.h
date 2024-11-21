#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleTextRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleTextRenderer();
	~TitleTextRenderer();

	void Initialize() override;
	void Update()     override;


	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
