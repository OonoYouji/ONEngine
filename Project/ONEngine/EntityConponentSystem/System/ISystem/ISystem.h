#pragma once


class ISystem {
public:

	virtual ~ISystem() {}

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

};