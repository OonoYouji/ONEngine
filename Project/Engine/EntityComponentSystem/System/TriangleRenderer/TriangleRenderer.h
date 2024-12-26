#pragma once

/// engine
#include "../ISystem/ISystem.h"

class TriangleRenderer : public ISystem {
public:

	TriangleRenderer() {}
	~TriangleRenderer() {}

	void Execute(size_t _entity, ECSManager* _manager) override;

private:

};

