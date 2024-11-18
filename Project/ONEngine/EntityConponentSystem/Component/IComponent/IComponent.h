#pragma once

class IComponent {
public:

	virtual ~IComponent() {}

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

};