#pragma once
#include <BaseGameObject.h>
#include <sstream>
#include <Sprite.h>
#include <AudioManager.h>

class EnemyManager : public BaseGameObject
{
public:
	EnemyManager(){ CreateTag(this); }
	~EnemyManager();

public:

	void Initialize()override;
	void Update()override;
	void FrontSpriteDraw()override;

	void EnemyPop();
	void LoadPopDate(const std::string& fileName, std::stringstream& command);
	void PopCommands();
	void PopInfinite();
	void WarningUpdate();

	void Debug()override;

	void SetGameManager(class GameManager* gameManager);

private:
	std::unique_ptr<Sprite> warningSprite_;
	Vector3 warningPosition_{};
	Vector3 warningSize_{};
	bool isWarning_ = false;

	const std::string kDirectoryPath_ = "./Resources/Texts/";
	class GameManager* pGameManager_ = nullptr;
	std::stringstream popCommand_;
	std::stringstream infiniteCommand_;
	std::list<std::tuple<int, float,int>> commands_;

	uint32_t popCount_ = 0;
	float popInterval_ = 0;
	bool isBreak_ = false;
	float currentInterval_ = 0;
	bool isWait_ = false;
	float waitTimer_ = 0;

	uint32_t maxPopCount_ = 5;
	bool isStop_ = false;

	bool isInfinteMode_ = false;

	AudioSource* alarm_ = nullptr;
};
