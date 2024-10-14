#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"
//std



class BossChasePlayer : public BaseBossBehavior {
public:
	//コンストラクタ
	BossChasePlayer(Boss* boss);
	~BossChasePlayer();
	//関数
	Vec3 QuaternionToEulerAngles(const Quaternion& q);
	Vec3 DealtWithZimbalLock(float x, const Quaternion& q);
	Quaternion ToQuaternion(const Vec3& eulerAngles);
	void Update()override;
private:
	//ストーカースピード(速い)
	float chaseSpeedMax_;
	//ストーカースピード(遅い)
	float chaseSpeedNormal_;
	//追っかける最小距離
	float chaseMinPos_;
	//追っかける限界距離
	float chaseMaxPos_;

};
