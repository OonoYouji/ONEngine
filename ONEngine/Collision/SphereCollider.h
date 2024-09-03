#pragma once

#include "BaseCollider.h"

class SphereCollider final : public BaseCollider {
public:

	SphereCollider(){}
	~SphereCollider() {}


	void Initialize(BaseGameObject* gameObject, Model* model) override;

	void Update() override;
	void Draw() override;

	bool IsCollision(MAYBE_UNUSED class BoxCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class AABBCollider* aabb) override;
	bool IsCollision(MAYBE_UNUSED class OBBCollider* obb) override;
	bool IsCollision(MAYBE_UNUSED SphereCollider* sphere) override;

private:

};