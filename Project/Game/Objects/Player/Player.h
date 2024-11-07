#pragma once
#include "GameObjectManager/BaseGameObject.h"

#include <memory>

class IPlayerBehavior;
class Player :
	public BaseGameObject{
public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;

	void Debug()override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;

	std::unique_ptr<IPlayerBehavior> currentBehavior_;

	Vector2 direction_;
	float speed_;

	float currentHP_;
	float maxHP_;

public:
	void SetDirection(const Vector2& direction){ direction_ = direction; }

	const Transform* GetTransform()const{ return pTransform_; }
	void SetScale(const Vector3 s){ pTransform_->scale = s; }
	void SetRotate(const Vector3 r){ pTransform_->rotate = r; }
	void SetPostion(const Vector3 pos){ pTransform_->position= pos; }

	float GetSpeed()const{ return speed_; }

	float GetCurrentHP() const { return currentHP_; }
	float GetMaxHP()     const { return maxHP_; }

};