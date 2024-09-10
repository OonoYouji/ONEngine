#pragma once

#include <GameObjectManager.h>


class GameManager final : public BaseGameObject {
public:

	GameManager() { CreateTag(this); }
	~GameManager() {}

	void Initialize() override;
	void Update() override;
	
	void AddPausedObject(BaseGameObject* object);

private:

	bool isPouse_ = false;
	bool prevIsPause_ = false;

	std::list<BaseGameObject*> pausedObjects_;

};