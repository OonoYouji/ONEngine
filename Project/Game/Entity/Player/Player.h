#pragma once

#include "Engine/Entity/Interface/IEntity.h"

class Player : public IEntity {
public:

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;

private:

	

};

