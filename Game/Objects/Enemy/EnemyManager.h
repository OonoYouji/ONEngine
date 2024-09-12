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
	void LoadPopDate(const std::string& fileName);
	void PopCommands();
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
	std::list<std::pair<int, int>> commands_;

	int32_t popCount_ = 0;
	uint32_t popInterval_ = 0;
	uint32_t currentInterval_ = 0;
	bool isWait_ = false;
	int32_t waitTimer_ = 0;

	AudioSource* alarm_ = nullptr;
};
