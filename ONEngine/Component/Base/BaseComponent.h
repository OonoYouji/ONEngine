#pragma once

class BaseGameObject;

class BaseComponent {
public:

	virtual ~BaseComponent() {}

	virtual void Initialize() {}

	virtual void Update() {}
	virtual void LastUpdate() {}

	virtual void BackSpriteDraw() {}
	virtual void Draw() {}
	virtual void FrontSpriteDraw() {}

	virtual void Debug() {}

	BaseGameObject* GetParent() const { return parent_; }
	void SetParent(BaseGameObject* parent);

	bool isActive = true;

private:
	BaseGameObject* parent_;
};