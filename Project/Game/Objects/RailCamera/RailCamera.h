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

	void SetGameCamera(GameCamera* _gameCamera);
	void SetShootingCourse(ShootingCourse* _shootingCourse);
	void SetDirectionalLight(class DirectionalLight* _directionalLight);


	float GetMovingTime()    const { return movingTime_; }
	float GetPreMovingTime() const { return preMovingTime_; }


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// move setting
	Vec3  moveDirection_{};      /// 移動している方向ベクトル
	Vec3  upDirection_{};
	float moveSpeed_;
	
	float movingTime_;           /// 移動の総合時間
	float preMovingTime_;        /// 移動の総合時間
	
	float nextMoveT_;            /// アンカーポイント間の移動に使うt
	float futureMoveT_;          /// アンカーポイント間の移動に使うt
	float futureTime_;           /// どれくらい先かを表す

	AnchorPoint nextAnchor_{};   /// 移動先のアンカーポイント(補完
	AnchorPoint futureAnchor_{}; /// 少し先のアンカーポイント(補完

	Vec3 rightDirection_;



	/// other class pointer
	ShootingCourse* pShootingCourse_ = nullptr;
    GameCamera*     pGameCamera_ = nullptr;
	class DirectionalLight* pDirectionalLight_ = nullptr;

};
