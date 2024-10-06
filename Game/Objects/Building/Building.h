#pragma once

#include <GameObjectManager.h>

class Building final : public BaseGameObject {
public:

	Building() { CreateTag(this); }
	~Building() {}

	void Initialize() override;
	void Update()     override;

private:

	float xRotationRate_ = 0.0f;
	float yRotationRate_ = 0.0f;

};