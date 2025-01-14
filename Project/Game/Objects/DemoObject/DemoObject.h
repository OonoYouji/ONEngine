#pragma once

#include <vector>
#include <memory>

/// base class
#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/AnimationRenderer/Skeleton.h"

class DemoObject final : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void TextMatrix(const Mat4& _matrix);

private:

	Quaternion q1, q2, identity, conj, inv, normal, mul1, mul2;
	float norm;

};
