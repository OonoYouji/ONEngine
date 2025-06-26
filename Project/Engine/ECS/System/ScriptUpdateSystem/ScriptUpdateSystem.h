#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// scriptの更新を行うシステム
/// /////////////////////////////////////////////////
class ScriptUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ScriptUpdateSystem();
	~ScriptUpdateSystem() override;

	void Update(class EntityComponentSystem* _entityComponentSystem) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================



};

