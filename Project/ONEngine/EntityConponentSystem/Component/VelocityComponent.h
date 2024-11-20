#pragma once

#include "Math/Vector3.h"

/// base class
#include "IComponent/IComponent.h"


class VelocityComponent : public IComponent {
public:
	VelocityComponent();
	~VelocityComponent() {}

	Vec3 velocity_;
};