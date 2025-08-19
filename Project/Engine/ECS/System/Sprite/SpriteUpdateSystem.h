#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// spriteの更新を行うシステム
/// ///////////////////////////////////////////////////
class SpriteUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteUpdateSystem() = default;
	~SpriteUpdateSystem() override = default;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================


};

