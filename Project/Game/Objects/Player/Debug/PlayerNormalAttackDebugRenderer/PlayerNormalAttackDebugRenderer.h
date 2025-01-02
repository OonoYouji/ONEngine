#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerNormalAttackDebugRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerNormalAttackDebugRenderer(class Player* _playerPtr);
	~PlayerNormalAttackDebugRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshInstancingRenderer*  meshInstancingRenderer_ = nullptr;
	class Player*                  pPlayer_                = nullptr;
	class PlayerNormalAttack*      pNormalAttakState_       = nullptr;
};
