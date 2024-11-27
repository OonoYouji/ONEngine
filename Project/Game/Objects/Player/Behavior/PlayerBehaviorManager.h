#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// game
#include "BasePlayerBehavior.h"


class PlayerBehaviorManager {
public:
	PlayerBehaviorManager(class Player* _player);
	~PlayerBehaviorManager();

	void Initialize();
	void Update();

	void AddBehavior(const std::string& _name, BasePlayerBehavior* _addBehavior);

	void Debugging();

private:
	class Player* pPlayer_ = nullptr;

	std::unordered_map<std::string, std::unique_ptr<BasePlayerBehavior>> behaviorMap_;
	BasePlayerBehavior* currentBehavior_ = nullptr;
};