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

	Quaternion rotation0, rotation1;
	Quaternion interpolate0, interpolate1, interpolate2, interpolate3, interpolate4;

};
