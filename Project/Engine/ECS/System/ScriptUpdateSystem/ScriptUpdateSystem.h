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

	void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================



};

