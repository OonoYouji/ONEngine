#pragma once

/// std
#include <vector>
#include <array>
#include <span>

#include "Math/Vector3.h"


struct MotionKeyframe {
	float time;

	Vec3 position;
	Vec3 rotate;
	Vec3 scale = Vec3::kOne;
};

class BaseMotion {
	friend class PlayerBehaviorManager;
public:

	BaseMotion(class Player* _player);
	~BaseMotion();

	void Start();

	void Update();

protected:

	class Player* pPlayer_ = nullptr;

	float maxTime_;             /// モーションの全体時間
	float currentTime_;         /// 今現在の時間
	float currentKeyframeTime_; /// 現在のkeyframeの時間

	std::vector<MotionKeyframe> keyframes_;
	size_t currentKeyframeIndex_;

	MotionKeyframe currentKeyframe_;

	std::vector<Vec3> positions_;
	std::vector<Vec3> rotates_;
	std::vector<Vec3> scales_;

	bool isActive_ = false;

public:

	const MotionKeyframe& GetMotionKeyframe() const { return currentKeyframe_; }

};