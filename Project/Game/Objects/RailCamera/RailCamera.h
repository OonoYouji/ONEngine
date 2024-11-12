#pragma once

/// std
#include <vector>
#include <array>

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"


/// 前方宣言
class GameCamera;



class RailCamera : public BaseGameObject {
public:

	RailCamera();
	~RailCamera();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void Reset();
	void Move();


	void SetGameCamera(GameCamera* _gameCamera);
	void SetShootingCourse(ShootingCourse* _shootingCourse);
	void SetDirectionalLight(class DirectionalLight* _directionalLight);


	float GetMovingTime()    const { return movingTime_; }
	float GetPreMovingTime() const { return preMovingTime_; }

	bool GetIsMoveEnd() const { return isMoveEnd_; }

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// move setting
	Vec3  moveDirection_{};      /// 移動している方向ベクトル
	Vec3  upDirection_{};
	float moveSpeed_;
	float moveSpeedRate_;

	float movingTime_;           /// 移動の総合時間
	float preMovingTime_;        /// 移動の総合時間

	float nextMoveT_;            /// アンカーポイント間の移動に使うt
	float futureMoveT_;          /// アンカーポイント間の移動に使うt
	float futureTime_;           /// どれくらい先かを表す

	AnchorPoint nextAnchor_{};   /// 移動先のアンカーポイント(補完
	AnchorPoint futureAnchor_{}; /// 少し先のアンカーポイント(補完

	Vec3 rightDirection_;
	size_t anchorPointArraySize_; /// アンカーポイントの配列サイズ

	/// 
	bool isMoveEnd_; /// 最後まで移動したか


	/// other class pointer
	ShootingCourse* pShootingCourse_ = nullptr;
	GameCamera* pGameCamera_ = nullptr;
	class DirectionalLight* pDirectionalLight_ = nullptr;


	std::vector<float> moveSpeedRates_;
};
