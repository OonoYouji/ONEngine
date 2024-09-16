#pragma once

class BaseGameObject;

class BaseComponent {
public:

	virtual ~BaseComponent() {}

	virtual void Initialize() {}
	virtual void Update() {}
	virtual void Draw() {}

	virtual void Debug() {}

	BaseGameObject* GetParent() const { return parent_; }
	void SetParent(BaseGameObject* parent);

private:
	BaseGameObject* parent_;
	bool isActive_ = true;
};