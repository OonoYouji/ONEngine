#pragma once

#include "Math/Vector3.h"

/// base class
#include "IComponent/IComponent.h"


class PositionComponent : public IComponent {
public:

	PositionComponent() {}
	~PositionComponent() {}

	void Initialize() override;
	void Update()     override;

public:
	Vec3 position_;
};