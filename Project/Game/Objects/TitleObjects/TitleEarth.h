#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleEarth final : public BaseGameObject {
public:

	TitleEarth() { CreateTag(this); }
	~TitleEarth() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

};
