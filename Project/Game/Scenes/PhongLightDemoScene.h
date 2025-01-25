#pragma once

/// engine
#include "SceneManager/BaseScene.h"


/// ===================================================
/// phong light の demo scene
/// ===================================================
class PhongLightDemoScene : public BaseScene {
public:

	PhongLightDemoScene() {}
	~PhongLightDemoScene() {}

	void Initialize() override;
	void Update()     override;
};

