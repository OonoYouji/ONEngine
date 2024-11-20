#pragma once

#include "Math/Vector3.h"

/// base class
#include "IComponent/IComponent.h"


class PositionComponent : public IComponent {
public:
	PositionComponent() {}
	~PositionComponent() {}

	Vec3 position_;
};