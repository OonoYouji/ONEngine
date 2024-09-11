#pragma once
#include <BaseGameObject.h>

class EnemyManager : public BaseGameObject
{
public:
	EnemyManager(){ CreateTag(this); }
	~EnemyManager(){}

public:

	void Initialize()override;
	void Update()override;

	void Debug()override;

	void SetGameManager(class GameManager* gameManager);

private:
	class GameManager* pGameManager_ = nullptr;
};
