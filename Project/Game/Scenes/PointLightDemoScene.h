#pragma once

/// engine
#include "SceneManager/BaseScene.h"

/// ===================================================
/// デモシーン
/// ===================================================
class PointLightDemoScene : public BaseScene {
public:

	PointLightDemoScene() {}
	~PointLightDemoScene() {}
	
	void Initialize() override;
	void Update()     override;

private:

	std::array<class PointLight*, 5> pointLights_ = {};

};