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
	void Debug()      override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;
	void OnCollisionStay(BaseGameObject* const _collision) override;

	void AddVariables();
	void LoadVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class BoxCollider* boxCollider_ = nullptr;

	class Player* pPlayer_ = nullptr;

	bool isCollisionEnter_ = false;
	bool isCollisionStay_ = false;


};
