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

	void Debugging();


	void AddMotion(BaseMotion* _motion);



	BaseMotion* GetMotion() const { return motions_.at("default").get(); }

	void SaveMotionToJson(const std::string& _fileName, BaseMotion* _motion);
private:

	/// ===================================================
	/// private : methods
	/// ===================================================


	void SelectEditMotion();


	/// <summary>
	/// モーションの編集
	/// </summary>
	/// <param name="_motion"></param>
	void MotionEdit(const std::string& _key, BaseMotion* _motion);

	/// <summary>
	/// キーフレームの編集
	/// </summary>
	/// <param name="_keyframes"></param>
	void KeyframeEdit(std::vector<MotionKeyframe>& _keyframes);


private:
	class Player* pPlayer_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<BaseMotion>> motions_;

	std::unordered_map<std::string, std::unique_ptr<BasePlayerBehavior>> behaviorMap_;
	BasePlayerBehavior* currentBehavior_ = nullptr;

	
	/// editor parameters
	int selectedEditMotionIndex_;
	std::string selectMotionKey_;

};