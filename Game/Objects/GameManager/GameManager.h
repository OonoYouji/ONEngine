#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameManager final : public BaseGameObject {
public:

	GameManager() { CreateTag(this); }
	~GameManager() {}

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;
	
	void Debug() override;

	void AddPausedObject(BaseGameObject* object);

private:

	bool isPouse_ = false;
	bool prevIsPause_ = false;

	std::list<BaseGameObject*> pausedObjects_;


	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> white2x2_;

	Vec4 white2x2Color_{ 1,1,1,1 };
	Vec4 spriteColor_{ 1,1,1,1 };
	Vec2 spriteSize_{ 640, 360 };

};