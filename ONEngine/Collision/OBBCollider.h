#pragma once

#include "BaseCollider.h"

class OBBCollider final : public BaseCollider {
public:

	OBBCollider() {}
	~OBBCollider() {}

	void Initialize(BaseGameObject* gameObject, Model* model);
	void Update() override;
	void Draw() override;

	bool IsCollision(MAYBE_UNUSED class BoxCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class AABBCollider* box) override;
	bool IsCollision(MAYBE_UNUSED OBBCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class SphereCollider* box) override;

private:

};