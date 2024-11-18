#pragma once


class IComponent {
public:

	virtual ~IComponent() {}

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

	void SetOwner(class Entity* _owner);
	class Entity* GetOwner() const { return owner_; }

private:
	class Entity* owner_;
};