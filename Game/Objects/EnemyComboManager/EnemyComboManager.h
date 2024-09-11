#pragma once
#include <GameObjectManager.h>
#include <Sprite.h>

class EnemyComboManager : public BaseGameObject
{
public:
	EnemyComboManager() { CreateTag(this); }
	~EnemyComboManager();
public:

	void Initialize()override;
	void Update()override;
	void FrontSpriteDraw()override;

	void Debug()override;


private:

	std::unique_ptr<Sprite> numberSprite_;
	std::unique_ptr<Sprite> comboSprite_;
	uint32_t comboCount_ = 0;
	uint32_t comboReceptionTime_ = 0;
	uint32_t interruptTime_ = 30;

};
