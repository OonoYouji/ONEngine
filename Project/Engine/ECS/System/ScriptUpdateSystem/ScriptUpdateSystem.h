#pragma once

/// std
#include <list>
#include <string>

/// externals
#include <mono/jit/jit.h>


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

	ScriptUpdateSystem(class ECSGroup* _ecs);
	~ScriptUpdateSystem() override;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/// 生成
	void MakeScriptMethod(MonoImage* _image, const std::string& _ecsGroupName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoImage* pImage_;

	MonoClass*  monoClass_;
	uint32_t    gcHandle_;
	MonoMethod* updateEntitiesMethod_;
	MonoMethod* addEntityMethod_;
	MonoMethod* addScriptMethod_;

};