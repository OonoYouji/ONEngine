#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerAttackCollider : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerAttackCollider();
	~PlayerAttackCollider();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class BoxCollider* collider_ = nullptr;

};
