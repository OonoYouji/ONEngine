#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultToTitle : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultToTitle();
	~ResultToTitle();

	void Initialize() override;
	void Update()     override;


	void ApplyVariables();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
