#pragma once

/// std
#include <string>
#include <unordered_map>
#include <functional>

/// object
#include "BossParts.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"

/// 前方宣言
class GameCamera;


/// ===================================================
/// ボスの動きの種類
/// ===================================================
enum BOSS_ANIMATION_ORDER {
	BOSS_ANIMATION_NONE,              /// 何もしない
	BOSS_ANIMATION_STANDBY,           /// 待機モーション
	BOSS_ANIMATION_ENTRY_CAMERA_MOVE, /// 出現 NO.1  カメラ移動
	BOSS_ANIMATION_ENTRY_RAISE_TUBE,  /// 出現 NO.2  チューブの振り上げ
	BOSS_ANIMATION_ENTRY_TUBE_DOWN,   /// 出現 NO.3  チューブの振り下ろし
	BOSS_ANIMATION_COUNT  /// 計測用
};


/// ===================================================
/// ボスの各パーツの名前
/// ===================================================
enum BOSS_PARTS_NAME {
	BOSS_PARTS_BODY,
	BOSS_PARTS_TUBU,
	BOSS_PARTS_HEAD,
	BOSS_PARTS_COUNT /// 計測用
};


/// ===================================================
/// ボスのアニメーション
/// ===================================================
class BossAnimation : public BaseGameObject {

	struct AnimationData {
		float time;
		float maxTime;
		float apmlitude;
	};

public:

	BossAnimation();
	~BossAnimation();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetGameCamera(GameCamera* _gameCamera);

	void SetAnimationIndex(BOSS_ANIMATION_ORDER _bossAnimationOrder);

private:

	std::vector<BossParts*>                          bossPartsArray_;
	std::vector<std::string>                         modelNameArray_;
	std::vector<std::function<void(AnimationData&)>> animationUpdateFunction_;
	std::vector<AnimationData>                       animationDataArray_;

	int currentAnimationIndex_ = 0;

	/// other pointer
	GameCamera* pGameCamera_ = nullptr;

};

