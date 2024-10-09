#pragma once

/// base
#include "GameObjectManager/GameObjectManager.h"


/// ===================================================
/// 前方宣言
/// ===================================================
class ShootingCourse;



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


	void SetShootingCourse(ShootingCourse* _shootingCourse);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// move setting
	Vec3  moveDirection_;
	float moveT_;

	/// other class pointer
	ShootingCourse* pShootingCourse_ = nullptr;



};