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

	std::unique_ptr<Sprite> oneSprite_;
	std::unique_ptr<Sprite> twoSprite_;
	std::unique_ptr<Sprite> threeSprite_;
	std::unique_ptr<Sprite> fourSprite_;
	std::unique_ptr<Sprite> fiveSprite_;
	std::unique_ptr<Sprite> sixSprite_;
	std::unique_ptr<Sprite> sevenSprite_;
	std::unique_ptr<Sprite> eightSprite_;
	std::unique_ptr<Sprite> nineSprite_;
	std::unique_ptr<Sprite> comboSprite_;
	uint32_t comboCount_ = 0;
	uint32_t comboReceptionTime_ = 0;
	uint32_t interruptTime_ = 30;

};
