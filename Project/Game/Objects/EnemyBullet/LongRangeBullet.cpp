#include "LongRangeBullet.h"

#include <algorithm>

#include "FrameManager/Time.h"
#include "Math/Easing.h"
#include "Math/Random.h"

LongRangeBullet::LongRangeBullet():IEnemyBullet(BulletType::LONG_RANGE_BULLET){
	CreateTag(this);
}

void LongRangeBullet::Initialize(){
	IEnemyBullet::Initialize();
}

void LongRangeBullet::Update(){
	IEnemyBullet::Update();

	Vector3 newPos = GetPosition();
	newPos += Vector3(velocityXZ_.x,0.0f,velocityXZ_.y) * Time::DeltaTime();
	SetPosition(newPos);
}