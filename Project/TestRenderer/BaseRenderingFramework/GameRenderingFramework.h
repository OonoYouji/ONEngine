#pragma once

#include "BaseRenderingFramework.h"

/// ===================================================
/// ゲームの描画フレームワーク
/// ===================================================
class GameRenderingFramework : public BaseRenderingFramework {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameRenderingFramework() {}
	~GameRenderingFramework() {}

	void Initialize() override;
	void Update()     override;

	
private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};