#pragma once

#include <GameObjectManager.h>

class Shake : public BaseGameObject {
public:
	Shake() { CreateTag(this); }
	~Shake() {}
	void Initialize() override;
	void Update() override;
	void Debug() override;
	void Start();
	void SetStartTime(float time);
private:
	float startTime_;
	float time_;
	float speed_;
};