#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
//#include "../Interface/IEntity.h"

/// ===================================================
/// デモ用エンティティ
/// ===================================================
class DemoEntity : public IEntity {
public:

	DemoEntity() {}
	~DemoEntity() {}

	void Initialize() override;
	void Update()     override;

private:
};

