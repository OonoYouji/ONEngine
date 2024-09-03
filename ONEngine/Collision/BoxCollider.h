#pragma once

#include "BaseCollider.h"
#include <Model.h>
#include <Vector3.h>
#include <Transform.h>

class BoxCollider : public BaseCollider {
public:

	BoxCollider() {}
	~BoxCollider() {}


	void Initialize(BaseGameObject* gameObejct, Model* model) override;
	void Update() override;
	void Draw() override;


	bool IsCollision(MAYBE_UNUSED BoxCollider* box) override;
	bool IsCollision(MAYBE_UNUSED class AABBCollider* aabb) override;
	bool IsCollision(MAYBE_UNUSED class OBBCollider* obb) override;
	bool IsCollision(MAYBE_UNUSED class SphereCollider* sphere) override;

private:

	std::vector<Vec3> GetVertices() const;

	void UpdateOrientatinos();

private:

	Vec3 size_;
	Vec3 orientatinos_[3];

	Model* cube_ = nullptr;

};