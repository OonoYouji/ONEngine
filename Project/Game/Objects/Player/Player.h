#pragma once

/// std
#include <vector>
#include <array>

/// base
#include "GameObjectManager/GameObjectManager.h"

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"


/// 前方宣言
class Reticle;


/// ===================================================
/// プレイヤーが操作するエンティティのクラス
/// ===================================================
class Player : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetReticle(Reticle* _reticle);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Reticle* pReticle_ = nullptr;

	/// bullet param
	float bulletSpeed_;

};