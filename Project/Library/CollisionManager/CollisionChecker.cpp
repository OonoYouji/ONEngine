#define NOMINMAX
#include "CollisionChecker.h"

/// std
#include <algorithm>

/// lib
#include "Math/Vector3.h"

/// collider
#include "ComponentManager/Collider/BaseCollider.h"
#include "ComponentManager/Collider/BoxCollider.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/Collider/CapsuleCollider.h"


bool CollisionChecker::BoxToBox(BoxCollider* a, BoxCollider* b) {
	///- 分離軸を計算
	std::vector<Vec3> axes{};

	///- 面法線を分離軸に
	for(uint32_t index = 0; index < 3; ++index) {
		axes.push_back(a->GetOrientations()[index]);
		axes.push_back(b->GetOrientations()[index]);
	}

	///- 面法線同士の外積を分離軸に
	for(uint32_t row = 0; row < 3; ++row) {
		for(uint32_t col = 0; col < 3; ++col) {
			Vec3 cross = Vec3::Cross(a->GetOrientations()[row], b->GetOrientations()[col]);
			if(cross.Len() > 1e-6f) {
				axes.push_back(cross);
			}
		}
	}

	std::vector<Vec3> v1 = a->GetVertices();
	std::vector<Vec3> v2 = b->GetVertices();

	///- 分離軸から二つのオブジェクトが離れているか計算
	for(auto& axis : axes) {

		axis = axis.Normalize();

		///- obb1の最小値と最大値
		float min1 = Vec3::Dot(Vec3::MinDotVector(axis, v1), axis);
		float max1 = Vec3::Dot(Vec3::MaxDotVector(axis, v1), axis);
		float diff1 = max1 - min1;

		///- obb2の最小値と最大値
		float min2 = Vec3::Dot(Vec3::MinDotVector(axis, v2), axis);
		float max2 = Vec3::Dot(Vec3::MaxDotVector(axis, v2), axis);
		float diff2 = max2 - min2;

		///- 差分の合計
		float sumSpan = diff1 + diff2;
		///- 二つのオブジェクトの最大値と最小値の差分
		float longSpan = std::max(max1, max2) - std::min(min1, min2);

		///- 離れている
		if(sumSpan < longSpan) {
			return false;
		}
	}

	return true;
}

bool CollisionChecker::SphereToSphere(SphereCollider* a, SphereCollider* b) {
	Vec3 distance = a->GetPosition() - b->GetPosition();
	float length = distance.Len();

	///- 二点間の距離が二つの球の半径より小さければtrue
	if(length < a->GetRadius() + b->GetRadius()) {
		return true;
	}

	return false;
}

bool CollisionChecker::CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b) {
	const Vec3& aDir   = a->GetDirection(); // カプセルAの線分ベクトル
	const Vec3& bDir   = b->GetDirection(); // カプセルAの線分ベクトル
	Vec3 aTobDirection = a->GetStartPosition() - b->GetStartPosition();

	float aLenSq = std::pow(aDir.Len(), 2.0f); // カプセルAの線分の長さの2乗
	float bLenSq = std::pow(bDir.Len(), 2.0f); // カプセルBの線分の長さの2乗

	float uu = Vec3::Dot(aDir, aDir);
	float vv = Vec3::Dot(bDir, bDir);
	float uv = Vec3::Dot(aDir, bDir);
	float uw = Vec3::Dot(aDir, aTobDirection);
	float vw = Vec3::Dot(bDir, aTobDirection);

	float denom = uu * vv - uv * uv; // 平行判定用の分母
	float s = (denom == 0.0f) ? 0.0f : (uv * vw - vv * uw) / denom;
	float t = (denom == 0.0f) ? 0.0f : (uu * vw - uv * uw) / denom;

	// sとtを線分の範囲にクランプ
	s = std::clamp(s, 0.0f, 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);

	// 最近接点を計算
	Vec3 closestPointA = a->GetStartPosition() + aDir * s;
	Vec3 closestPointB = b->GetStartPosition() + bDir * t;

	// 最近接点間の距離を求める
	Vec3 distanceVec = closestPointA - closestPointB;
	float distanceSq = std::pow(distanceVec.Len(), 2.0f);

	// カプセルの半径の合計
	float radiusSum = a->GetRadius() + b->GetRadius();

	// 距離が半径の合計以下なら当たっている
	return distanceSq <= (radiusSum * radiusSum);
}

bool CollisionChecker::BoxToSphere(BoxCollider* box, SphereCollider* sphere) {
	return false;
}

bool CollisionChecker::BoxToCapsule(BoxCollider* box, CapsuleCollider* capsule) {
	return false;
}

bool CollisionChecker::SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule) {
	return false;
}
