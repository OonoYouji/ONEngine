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

	void CameraOffsetRotateUpdate(const Vec3& _rotateValue);


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
	Vec3 cameraOffsetRotate_;   /// 常に更新されている回転
	Vec3 cameraTargetRotate_;   /// ターゲットすると0になる回転
	Vec3 cameraNextPosition_;   /// カメラの次の座標

	Vec3 playerToEnemyVector_;  /// オブジェクト間のベクトル
	Vec3 cameraToEnemyVector_;  /// オブジェクト間のベクトル
	Vec3 cameraToPlayerVector_; /// オブジェクト間のベクトル

	float missTheTargetLenght_; /// ターゲットが外れる距離
	bool  isLockOn_;            /// ロックオンする、しているフラグ
	bool  prevIsLockOn_;
	float lockOnLenghtScaleFactor_; /// ロックオンしたとき距離の倍率

	/// quaternionの補間用の変数
	Quaternion cameraToPlayerQuaternion_;
	Quaternion cameraToEnemyQuaternion_;
	float      quaternionLerpTime_;  /// 線形補完用の時間
	float      quaternionLerpSpeed_; /// 線形補完のスピード

	Vec3 cameraMoveSpeedVector_; /// カメラの移動量ベクトル


	/// 追尾するオブジェクト
	BaseGameObject* pTargetObject_;
	Vec3            targetPosition_;
	Vec3            saveTargetPosition_;


	/// rotateのリセット用
	Vec3 saveCameraOffsetRotate_;
	Vec3 saveCameraTargetRotate_;

	float playerToEnemyRotateY_;
	bool  isTargetingActive_;
	float toTargetLerpTime_;
	float toTargetLerpMaxTime_;

	bool isTargetLost_; /// ターゲットをはずした

};
