#pragma once

/// engine
#include "Engine/Scene/ISceneFactory.h"

/// ///////////////////////////////////////////////////
/// シーンの生成を行うクラス
/// ///////////////////////////////////////////////////
class SceneFactory final : public ISceneFactory {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneFactory() = default;
	~SceneFactory() = default;

	void Initialize() override;

};

