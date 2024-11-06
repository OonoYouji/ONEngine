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
	
	/// 必要な値の確保
	const Vec3& aDir   = a->GetDirection(); // カプセルAの線分ベクトル
	const Vec3& bDir   = b->GetDirection(); // カプセルAの線分ベクトル
	Vec3 aTobDirection = b->GetStartPosition() - a->GetStartPosition(); // a -> bへのベクトル

	// 最近接点を計算
	float dotAToB = Vec3::Dot(aDir, +aTobDirection);
	float dotBToA = Vec3::Dot(bDir, -aTobDirection);

	dotAToB = std::clamp(dotAToB, 0.0f, a->GetLenght());
	dotBToA = std::clamp(dotBToA, 0.0f, b->GetLenght());

	Vec3 closestPointA = a->GetStartPosition() + aDir * dotAToB;
	Vec3 closestPointB = b->GetStartPosition() + bDir * dotBToA;

	// 最近接点間の距離を求める
	Vec3  distanceVec = closestPointA - closestPointB;
	float radiusSum = a->GetRadius() + b->GetRadius();

	// 距離が半径の合計以下なら当たっている
	return distanceVec.Len() <= radiusSum;
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
