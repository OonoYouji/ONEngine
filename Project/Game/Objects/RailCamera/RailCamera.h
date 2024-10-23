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


	/// <summary>
	/// アンカーポイントの配列と現在の位置を表す媒介変数で現在地のワールド座標を計算する
	/// </summary>
	/// <param name="anchorPointArray">: アンカーポイントの配列 </param>
	/// <param name="t">               : 現在地を表す媒介変数   </param>
	/// <returns>return                : 現在地のワールド座標   </returns>
	AnchorPoint SplinePosition(const std::vector<AnchorPoint>& anchorPointArray, float t) const;

	/// <summary>
	/// スプライン曲線の座標を補完する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	AnchorPoint SplineInterpolation(const std::array<AnchorPoint, 4>& anchorPointArray, float t) const;



	void SetGameCamera(GameCamera* _gameCamera);
	void SetShootingCourse(ShootingCourse* _shootingCourse);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// move setting
	Vec3  moveDirection_{};      /// 移動している方向ベクトル
	Vec3  upDirection_{};
	float movingTime_;           /// 移動の総合時間
	float nextMoveT_;            /// アンカーポイント間の移動に使うt
	float futureMoveT_;          /// アンカーポイント間の移動に使うt
	float futureTime_;           /// どれくらい先かを表す
	AnchorPoint nextAnchor_{};   /// 移動先のアンカーポイント(補完
	AnchorPoint futureAnchor_{}; /// 少し先のアンカーポイント(補完

	Vec3 rightDirection_;

	/// other class pointer
	ShootingCourse* pShootingCourse_ = nullptr;
    GameCamera*     pGameCamera_ = nullptr;

};
