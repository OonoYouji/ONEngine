#pragma once

/// engine
#include "Engine/Scene/ISceneFactory.h"

/// ===================================================
/// シーンの生成を行うクラス
/// ===================================================
class SceneFactory final : public ISceneFactory {
public:

	SceneFactory() = default;
	~SceneFactory() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

};

