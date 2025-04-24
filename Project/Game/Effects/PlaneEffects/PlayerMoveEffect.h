#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////////////
/// プレイヤーが歩くときのエフェクト
/// ////////////////////////////////////////////////////////
class PlayerMoveEffect : public IEntity {
public:
	/// ============================================================
	/// public : methods
	/// ============================================================

	PlayerMoveEffect() = default;
	~PlayerMoveEffect() = default;

	void Initialize() override;
	void Update() override;

private:
	/// ============================================================
	/// private : objects
	/// ============================================================

	class Effect* effect_ = nullptr;

};

