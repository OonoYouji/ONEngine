#pragma once

#include "BaseCollider.h"

class AABBCollider final : public BaseCollider {
public:

	AABBCollider(){}
	~AABBCollider() {}

	void Initialize(BaseGameObject* gameObject, Model* model);
	void Update() override;
	void Draw() override;

	bool IsCollision(MAYBE_UNUSED class BoxCollider* box) override;
	bool IsCollision(MAYBE_UNUSED AABBCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class OBBCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class SphereCollider* box) override;

private:

};