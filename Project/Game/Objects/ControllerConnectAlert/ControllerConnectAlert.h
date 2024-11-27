#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ControllerConnectAlert : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ControllerConnectAlert();
	~ControllerConnectAlert();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;


};
