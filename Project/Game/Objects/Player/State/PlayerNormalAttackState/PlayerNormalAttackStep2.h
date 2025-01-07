#pragma once

/// std
#include <array>
#include <string>

#include "../IPlayerState/IPlayerState.h"

#include "Math/Vector3.h"

class PlayerNormalAttackStep2 : public IPlayerState {
public:

	PlayerNormalAttackStep2(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerNormalAttackStep2();

	void Start() override;
	void Update() override;
	void Exit() override;

	bool IsEnd() override;
	int NextStateIndex() override;

	void Debug() override;
	void Save() override;
	void Load() override;

	Vec3 Vec3ArrayLerp(const std::array<Vec3, 4>& _array, float _t);

private:

	const std::string filePath_ = "./Resources/Parameters/Objects/PlayerNormalAttackStep2.json";
	std::array<Vec3, 4> defaultAttackPos_;
	std::array<Vec3, 4> attackPos_;

	Vec3 startPosition_;
	Vec3 direction_;

	float maxTime_;
	float currentTime_;

	bool isEnd_;
	bool isInputNextAttack_;


};