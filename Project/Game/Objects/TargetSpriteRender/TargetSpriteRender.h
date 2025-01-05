#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TargetSpriteRender : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TargetSpriteRender(class GameCamera* _gameCamera);
	~TargetSpriteRender();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;
	class GameCamera*   pGameCamera_  = nullptr;

};
