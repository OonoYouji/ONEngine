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


	void SetShootingCourse(ShootingCourse* _shootingCourse);

	/// <summary>
	/// アンカーポイントの配列と現在の位置を表す媒介変数で現在地のワールド座標を計算する
	/// </summary>
	/// <param name="anchorPointArray">: アンカーポイントの配列 </param>
	/// <param name="t">               : 現在地を表す媒介変数   </param>
	/// <returns>return                : 現在地のワールド座標   </returns>
	AnchorPoint SplinePosition(const std::vector<AnchorPoint>& anchorPointArray, float t);

	/// <summary>
	/// スプライン曲線の座標を補完する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	AnchorPoint SplineInterpolation(const std::array<AnchorPoint, 4>& anchorPointArray, float t);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// move setting
	Vec3  moveDirection_{};    /// 移動している方向ベクトル
	float movingTime_;         /// 移動の総合時間
	float moveT_;              /// アンカーポイント間の移動に使うt
	AnchorPoint nextAnchor_{}; /// 移動先のアンカーポイント(補完

	/// other class pointer
	ShootingCourse* pShootingCourse_ = nullptr;



};