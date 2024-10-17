#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"
//std



class BossChasePlayer : public BaseBossBehavior {
public:
	//コンストラクタ
	BossChasePlayer(Boss* boss);
	~BossChasePlayer();

	Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t);
	Quaternion Slerp(const Quaternion& start, Quaternion end, float t);



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
