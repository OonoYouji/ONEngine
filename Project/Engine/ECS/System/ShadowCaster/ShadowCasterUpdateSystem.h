#pragma once

/// engine
#include "Engine/ECS/System/Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// ShadowCasterを更新するためのシステム
/// ///////////////////////////////////////////////////
namespace ONEngine {

class ShadowCasterUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	ShadowCasterUpdateSystem();
	~ShadowCasterUpdateSystem() override;
	
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	void Update(class ECSGroup* _ecs);

};


} /// ONEngine
