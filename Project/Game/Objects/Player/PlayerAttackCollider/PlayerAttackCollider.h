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
	float damageValue_;

public:

	void SetColliderActive(bool _isActive);

	void SetDamageValue(float _value) { damageValue_ = _value; }
	float GetDamageValue() const { return damageValue_; }
};
