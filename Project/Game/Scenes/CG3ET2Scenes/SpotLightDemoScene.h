#pragma once
#include "SceneManager/BaseScene.h"

class SpotLightDemoScene : public BaseScene {
public:

	SpotLightDemoScene() {}
	~SpotLightDemoScene() {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新
	void Update() override;


private:

	std::array<class SpotLight*, 5> spotLights_ = {};
};

