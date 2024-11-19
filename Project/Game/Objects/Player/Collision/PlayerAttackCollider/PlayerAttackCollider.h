#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerAttackCollider : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerAttackCollider(class Player* _player);
	~PlayerAttackCollider();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class BoxCollider* boxCollider_ = nullptr;

	class Player* pPlayer_ = nullptr;

};
