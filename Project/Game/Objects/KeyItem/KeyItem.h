#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


/// /////////////////////////////////////////////////////////////////
/// KeyItemクラス
/// /////////////////////////////////////////////////////////////////
class KeyItem : public IEntity {
public:
	KeyItem();
	~KeyItem();

	void Initialize() override;
	void Update() override;

};

