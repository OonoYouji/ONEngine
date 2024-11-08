#pragma once

/// base class
#include "ComponentManager/Base/BaseComponent.h"


class NumberRenderer final : public BaseComponent {
public:

	NumberRenderer();
	~NumberRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

private:

};
