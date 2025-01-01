#pragma once

/// std
#include <string>

/// user
#include "../IPlayerState/IPlayerState.h"

class PlayerProtectionState : public IPlayerState {
public:

	PlayerProtectionState(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerProtectionState();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	bool IsEnd()  override;
	int NextStateIndex() override;

	void Debug() override;
	void Save()  override;
	void Load()  override;

private:

	float time_;
	float protectionValue_; /// シールドの耐久力

	const std::string filePath_ = "./Resources/Parameters/Objects/PlayerProtectionState.json";
	const std::string groupName_ = "PlayerProtectionState";


public:


	/// <summary>
	/// 耐久値の取得
	/// </summary>
	/// <returns></returns>
	float GetProtectionValue() const { return protectionValue_; }

};

