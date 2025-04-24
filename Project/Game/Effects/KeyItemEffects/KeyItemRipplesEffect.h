#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class KeyItemRipplesEffect : public IEntity {
public:

	KeyItemRipplesEffect();
	~KeyItemRipplesEffect();

	void Initialize() override;
	void Update() override;

private:


};

