#pragma once

#include"BaseBuilding.h"

class Building : public BaseBuilding {
public:

	Building() { CreateTag(this); }
	~Building() {}

	void Initialize()override;
	void Update() override;

	void Debug() override;

private:
	
	
};