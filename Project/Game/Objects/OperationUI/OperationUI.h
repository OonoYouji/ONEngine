#pragma once

#include "GameObjectManager/BaseGameObject.h"

class OperationUI : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	OperationUI();
	~OperationUI();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;

};
