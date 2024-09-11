#pragma once
#include <BaseGameObject.h>
#include <sstream>

class EnemyManager : public BaseGameObject
{
public:
	EnemyManager(){ CreateTag(this); }
	~EnemyManager(){}

public:

	void Initialize()override;
	void Update()override;

	void EnemyPop();
	void LoadPopDate(const std::string& fileName);
	void PopCommands();

	void Debug()override;

	void SetGameManager(class GameManager* gameManager);

private:
	const std::string kDirectoryPath_ = "./Resources/Texts/";
	class GameManager* pGameManager_ = nullptr;
	std::stringstream popCommand_;
	std::list<std::pair<int, int>> commands_;
	int32_t popCount_ = 0;
	uint32_t popInterval_ = 0;
	uint32_t currentInterval_ = 0;
	bool isWait_ = false;
	int32_t waitTimer_ = 0;
};
