#pragma once

/// std
#include <vector>
#include <array>

/// base
#include "GameObjectManager/GameObjectManager.h"

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"



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


	/// <summary>
	/// シューティングコースタークラスのセット
	/// </summary>
	/// <param name="_shootingCourse"></param>
	void SetShootingCourse(ShootingCourse* _shootingCourse);

	/// <summary>
	/// スタート地点に戻る
	/// </summary>
	void Reset();



private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// bullet param
	float bulletSpeed_;

};