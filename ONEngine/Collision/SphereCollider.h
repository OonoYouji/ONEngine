#pragma once

#include "BaseCollider.h"

class SphereCollider final : public BaseCollider {
public:

	SphereCollider(){}
	~SphereCollider() {}


	void Initialize(BaseGameObject* gameObject, Model* model) override;

	void Update() override;
	void Draw() override;

	float GetRadius() const { return radius_; }

private:

	float radius_ = 0.0f;
	class Model* sphere_ = nullptr;

};