#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultSprite : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultSprite();
	~ResultSprite();

	void Initialize() override;
	void Update()     override;


	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
