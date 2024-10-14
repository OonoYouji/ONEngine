#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"


class BossSlurp : public BaseBossBehavior {
public:
	//コンストラクタ
	BossSlurp(Boss* boss);
	~BossSlurp();
	void Update()override;

	std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius);
private:
	float easingTime_;
	float easingTimeMax_;
	float easeDirection_;
	float MinDistance_;
};
