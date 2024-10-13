#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"
//std



class BossChasePlayer : public BaseBossBehavior {
public:
	//コンストラクタ
	BossChasePlayer(Boss* boss);
	~BossChasePlayer();
	//関数
	std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition,const float&radius);
	Vec3 QuaternionToEulerAngles(const Quaternion& q);
	Quaternion ToQuaternion(const Vec3& eulerAngles);
	void Update()override;
private:
	//ストーカースピード(速い)
	float chaseSpeedMax_;
	//ストーカースピード(遅い)
	float chaseSpeedNormal_;
	//追っかける限界距離
	float chaseMinPos_;
};
