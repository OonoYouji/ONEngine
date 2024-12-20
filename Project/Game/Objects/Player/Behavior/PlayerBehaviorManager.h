#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// game
#include "BasePlayerBehavior.h"
#include "../Motion/BaseMotion.h"
#include <vector>


class PlayerBehaviorManager {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBehaviorManager(class Player* _player);
	~PlayerBehaviorManager();

	void Initialize();
	void Update();

	void AddBehavior(const std::string& _name, BasePlayerBehavior* _addBehavior);

	void AddMotion(BaseMotion* _motion);

	void SaveMotionToJson(const std::string& _fileName, BaseMotion* _motion);
private:

	/// ===================================================
	/// private : methods
	/// ===================================================


private:
	
	/// ===================================================
	/// private : object
	/// ===================================================

	class Player* pPlayer_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<BaseMotion>> motions_;

	std::unordered_map<std::string, std::unique_ptr<BasePlayerBehavior>> behaviorMap_;
	BasePlayerBehavior* currentBehavior_ = nullptr;

	
	/// editor parameters
	int selectedEditMotionIndex_;
	std::string selectMotionKey_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// 引数のモーションを得る
	/// </summary>
	/// <param name="_motionKey"></param>
	/// <returns></returns>
	BaseMotion* GetMotion(const std::string& _motionKey) const {
		return motions_.at(_motionKey).get();
	}

	/// <summary>
	/// 現在のモーションを得る
	/// </summary>
	/// <returns></returns>
	BaseMotion* GetCurrentMotion() const {
		return motions_.at(selectMotionKey_).get();
	}

	/// <summary>
	/// モーションのキーを得る
	/// </summary>
	/// <returns></returns>
	const std::string& GetMotionKey() const {
		return selectMotionKey_;
	}

};