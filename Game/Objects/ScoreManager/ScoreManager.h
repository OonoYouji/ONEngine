#pragma once
#include <GameObjectManager.h>
#include <Sprite.h>

class ScoreManager : public BaseGameObject {
public:
	ScoreManager(){ CreateTag(this); }
	~ScoreManager();

public:

	void Initialize()override;
	void Update()override;
	void FrontSpriteDraw()override;

	void Debug()override;

	void CalScoreSprite();

private:

	std::vector<std::unique_ptr<Sprite>> digitOneSprites_;
	std::vector<std::unique_ptr<Sprite>> digitTwoSprites_;
	std::vector<std::unique_ptr<Sprite>> digitThreeSprites_;
	std::vector<std::unique_ptr<Sprite>> digitFourSprites_;
	std::vector<std::unique_ptr<Sprite>> digitFiveSprites_;
	std::vector<std::unique_ptr<Sprite>> digitSixSprites_;

	uint32_t scoreCount_ = 0;
	float highPoint_ = 0.0f;
	uint32_t baseScore_ = 100;

	int oneCount_ = 0;
	int twoCount_ = 0;
	int threeCount_ = 0;
	int fourCount_ = 0;
	int fiveCount_ = 0;
	int sixCount_ = 0;

};
