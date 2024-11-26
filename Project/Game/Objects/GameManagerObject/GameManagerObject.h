#pragma once

/// std
#include <unordered_map>


#include "GameObjectManager/BaseGameObject.h"

#include "CustomMath/Flag.h"


class GameManagerObject : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameManagerObject();
	~GameManagerObject();

	void Initialize() override;
	void Update()     override;


	void SetEnemy(class Enemy* _enemy);
	void SetPlayer(class Player* _player);


	static const Flag& GetFlag(const std::string& _key);
	static void SetFlag(const std::string& _key, bool value);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	static std::unordered_map<std::string, Flag> flags_;


	class Enemy*  pEnemy_  = nullptr;
	class Player* pPlayer_ = nullptr;

};
