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

	void LockOnUpdate();


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
	Vec3 cameraOffsetRotate_;

	Vec3 playerToEnemyVector_; /// オブジェクト間のベクトル
	Vec3 cameraToEnemyVector_; /// オブジェクト間のベクトル
	Vec3 cameraToPlayerVector_;

	float missTheTargetLenght_; /// ターゲットが外れる距離
	bool  isLockOn_;            /// ロックオンする、しているフラグ
	bool  prevIsLockOn_;

	/// quaternionの補間用の変数
	Quaternion toPlayerQuaternion_;
	Quaternion toEnemyQuaternion_;
	float      quaternionLerpTime_;  /// 線形補完用の時間
	float      quaternionLerpSpeed_; /// 線形補完のスピード

	Vec3 cameraMoveSpeedVector_; /// カメラの移動量ベクトル

};
