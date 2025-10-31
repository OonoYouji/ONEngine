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

	/// エンティティとコンポーネントをC#に追加
	void AddAllEntitiesAndComponents(class ECSGroup* _ecsGroup);
	bool AddEntityToScript(class GameEntity* _entity);

	/// @brief EcsGroupの更新を呼び出す
	void CallUpdateEcsGroup();

	/// 生成
	void MakeScriptMethod(MonoImage* _image, const std::string& _ecsGroupName);

	/// 解放
	void ReleaseGCHandle();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoClass* monoClass_;
	uint32_t    gcHandle_;
	MonoMethod* updateEntitiesMethod_;
	MonoMethod* addEntityMethod_;
	MonoMethod* addScriptMethod_;

};



/// /////////////////////////////////////////////////
/// デバッグ用のスクリプト更新システム
/// /////////////////////////////////////////////////
class DebugScriptUpdateSystem : public ScriptUpdateSystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DebugScriptUpdateSystem(class ECSGroup* _ecs);
	~DebugScriptUpdateSystem() override;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;
};