#pragma once

/// std
#include <list>

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

	void RuntimeUpdate(class ECSGroup* _ecs) override;

	void RecursivePushBackScript(class GameEntity* _entity);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<class Script*> pScripts_;


};

