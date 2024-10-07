#pragma once

#include <BaseScene.h>

class Scene_Clear final : public BaseScene {
public:
	Scene_Clear() {}
	~Scene_Clear() {}

	void Initialize() override;
	void Update() override;
};