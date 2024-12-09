#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// game
#include "BasePlayerBehavior.h"
#include "../Motion/BaseMotion.h"


class PlayerBehaviorManager {
public:
	PlayerBehaviorManager(class Player* _player);
	~PlayerBehaviorManager();

	void Initialize();
	void Update();

	void AddBehavior(const std::string& _name, BasePlayerBehavior* _addBehavior);

	void Debugging();


	void AddMotion(BaseMotion* _motion);

	void MotionEdit(BaseMotion* _motion);

	void SelectEditMotion();

	BaseMotion* GetMotion() const { return tmp.get(); }

private:
	class Player* pPlayer_ = nullptr;
	std::unique_ptr<BaseMotion> tmp;
	std::unordered_map<std::string, BaseMotion> motions_;

	std::unordered_map<std::string, std::unique_ptr<BasePlayerBehavior>> behaviorMap_;
	BasePlayerBehavior* currentBehavior_ = nullptr;
};