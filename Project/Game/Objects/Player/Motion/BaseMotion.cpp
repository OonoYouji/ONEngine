#define NOMINMAX
#include "BaseMotion.h"

/// std
#include <cmath>
#include <algorithm>

/// engine
#include "Debugger/Assertion.h"
#include "FrameManager/Time.h"

namespace {

	Vec3 SplineInterpolation(const std::array<Vec3, 4>& vs, float t) {
		const float s = 0.5f;

		float t2 = t * t; // t^2
		float t3 = t2 * t; // t^3

		Vec3 e3 = -vs[0] + (vs[1] * 3.0f) - (vs[2] * 3.0f) + vs[3];
		Vec3 e2 = (vs[0] * 2.0f) - (vs[1] * 5.0f) + (vs[2] * 4.0f) - vs[3];
		Vec3 e1 = -vs[0] + vs[2];
		Vec3 e0 = vs[1] * 2.0f;

		return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * s;
	}

	Vec3 CaclationSpline(const std::vector<Vec3>& vertices, float t) {
		Assert(vertices.size() >= 4, "制御点は4点以上必要です");

		size_t division = vertices.size() - 1;
		float areaWidth = 1.0f / static_cast<float>(division);

		///- 区間内の始点を0.0f, 終点を1.0f としたときの現在位置
		float t_2 = std::fmod(t, areaWidth) * division;
		t_2 = std::clamp(t_2, 0.0f, 1.0f);

		///- 区画番号
		size_t index = static_cast<size_t>(t / areaWidth);
		if(index > vertices.size() - 2) {
			index = vertices.size() - 2;
		}

		size_t indices[4]{
			index - 1,
			index,
			index + 1,
			index + 2
		};

		if(index == 0) {
			indices[0] = indices[1];
		}

		if(indices[3] >= vertices.size()) {
			indices[3] = indices[2];
		}

		const Vec3& p0 = vertices[indices[0]];
		const Vec3& p1 = vertices[indices[1]];
		const Vec3& p2 = vertices[indices[2]];
		const Vec3& p3 = vertices[indices[3]];

		return SplineInterpolation({ p0, p1, p2, p3 }, t_2);
	}

	
} /// namespace




BaseMotion::BaseMotion(Player* _player) {
	keyframes_ = {};
	keyframes_.push_back({});
	keyframes_.push_back({});
	keyframes_.push_back({});
	keyframes_.push_back({});
}

BaseMotion::~BaseMotion() {}

void BaseMotion::Start() {
	currentTime_ = 0.0f;
	isActive_ = true;

	currentKeyframeTime_ = 0.0f;
	currentKeyframeIndex_ = 0;

	/// アニメーションの最大時間を計算
	maxTime_ = 0.0f;
	for(auto& keyframe : keyframes_) {
		maxTime_ += keyframe.time;
	}
}


void BaseMotion::Update() {

	if(!isActive_) {
		return;
	}

	/// 時間を増やす
	currentTime_ += Time::DeltaTime();
	currentKeyframeTime_ += Time::DeltaTime();
	currentTime_ = std::min(currentTime_, maxTime_);

	/// すべてクリアする
	positions_.clear();
	rotates_.clear();
	scales_.clear();

	for(auto& keyframe : keyframes_) {
		positions_.push_back(keyframe.position);
		rotates_.push_back(keyframe.rotate);
		scales_.push_back(keyframe.scale);
	}

	/// lerpTの計算,
	float t = static_cast<float>(currentKeyframeIndex_) / static_cast<float>(keyframes_.size() - 1);
	t += (1.0f / static_cast<float>(keyframes_.size() - 1)) *
		(currentKeyframeTime_ / (keyframes_[currentKeyframeIndex_].time == 0.0f ? 1.0f : keyframes_[currentKeyframeIndex_].time));

	currentKeyframe_.position = CaclationSpline(positions_, t);
	currentKeyframe_.rotate   = CaclationSpline(rotates_,   t);
	currentKeyframe_.scale    = CaclationSpline(scales_,    t);


	/// 次のkeyframeに行くかどうか
	if(currentKeyframeTime_ >= keyframes_[currentKeyframeIndex_].time) {
		/// すべてのkeyframeを回った
		if(currentKeyframeIndex_ == keyframes_.size() - 1) {
			isActive_ = false;
		} else {
			currentKeyframeIndex_++;
			currentKeyframeTime_ = 0.0f;
		}
	}

	/*/// 終わったのか確認
	if(currentTime_ >= maxTime_) {
		isActive_ = false;
	}*/

}
