#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TopDownCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TopDownCamera(class GameCamera* _gameCamera, BaseGameObject* _targetObject);
	~TopDownCamera();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GameCamera* pGameCamera_  = nullptr;
	BaseGameObject*   targetObject_ = nullptr;
};
