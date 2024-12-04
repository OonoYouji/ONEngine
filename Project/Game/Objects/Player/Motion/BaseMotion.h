#pragma once

#include "../Player.h"

class BaseMotion {
public:

	BaseMotion(Player* _player);
	~BaseMotion();

protected:

	Player* pPlayer_ = nullptr;

	float maxTime_;      /// モーションの全体時間
	float currentTime_;  /// 今現在の時間

	Vec3 startPosition_; /// 開始位置
	Vec3 endOffset_;     /// 移動先

};