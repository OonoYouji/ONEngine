#pragma once

#include <GameObjectManager.h>

class ShootingCourse final : public BaseGameObject {
public:

	ShootingCourse() { CreateTag(this); }
	~ShootingCourse() {}

	void Initialize() override;
	void Update()     override;

private:



};