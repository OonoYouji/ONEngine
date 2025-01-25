#pragma once

/// engine
#include "SceneManager/BaseScene.h"

/// ===================================================
/// デモシーン
/// ===================================================
class DemoScene : public BaseScene {
public:

	DemoScene() {}
	~DemoScene() {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新
	void Update() override;

private:


};

