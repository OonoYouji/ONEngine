#pragma once

/// std
#include <string>
#include <unordered_map>
#include <functional>

/// object
#include "BossParts.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"


/// ===================================================
/// ボスの動きの種類
/// ===================================================
enum BOSS_ANIMATION_ORDER {
	BOSS_ANIMATION_NONE,  /// 何もしない
	BOSS_ANIMATION_ENTRY, /// 出現
	BOSS_ANIMATION_COUNT  /// 計測用
};


/// ===================================================
/// ボスの各パーツの名前
/// ===================================================
enum BOSS_PARTS_NAME {
	BOSS_PARTS_BODY,
	BOSS_PARTS_HEAD,
	BOSS_PARTS_TUBU,
	BOSS_PARTS_COUNT /// 計測用
};


/// ===================================================
/// ボスのアニメーション
/// ===================================================
class BossAnimation : public BaseGameObject {
public:

	BossAnimation();
	~BossAnimation();

	void Initialize() override;
	void Update()     override;

private:

	std::vector<BossParts*>                bossPartsArray_;
	std::vector<std::string>               modelNameArray_;
	std::vector<std::function<void(void)>> animationUpdateFunction_;

	int currentAnimationIndex_ = 0;

};

