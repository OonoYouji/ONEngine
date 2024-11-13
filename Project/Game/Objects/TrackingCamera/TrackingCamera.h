#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TrackingCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TrackingCamera(class GameCamera* _gameCamera, class Player* _player, class Enemy* _enemy);
	~TrackingCamera();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void LockOnToEnemy();

	void LockOnToPlayer();

	/// <summary>
	/// 値の適用、保存
	/// </summary>
	void ApplyVariables();
	void AddVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	/// other class pointer
	class GameCamera* pGameCamera_ = nullptr; /// ゲームカメラへのポインタ
	class Player*     pPlayer_     = nullptr; /// プレイヤーへのポインタ
	class Enemy*      pEnemy_      = nullptr;

	Vec3 cameraOffsetPosition_; /// プレイヤーからどのくらい離れているか

	Vec3 playerToEnemyVector_; /// オブジェクト間のベクトル
	Vec3 cameraToEnemyVector_; /// オブジェクト間のベクトル

	float missTheTargetLenght_; /// ターゲットが外れる距離
	bool  isLockOn_;            /// ロックオンする、しているフラグ

};
