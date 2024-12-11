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

	Vec3 from0;
	Vec3 to0;
	
	Vec3 from1;
	Vec3 to1;

	Mat4 matRotate0, matRotate1, matRotate2;

};
