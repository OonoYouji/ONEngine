#define NOMINMAX
#include "BaseCollider.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <GameObjectManager.h>
#include <Model.h>

#include "BoxCollider.h"
#include "SphereCollider.h"


void BaseCollider::Initialize(BaseGameObject* gameObject, [[maybe_unused]] Model* model) {
	gameObject_ = gameObject;
}

bool BaseCollider::IsCollision(MAYBE_UNUSED BaseCollider* other) {

	BoxCollider* boxA = dynamic_cast<BoxCollider*>(this);
	BoxCollider* boxB = dynamic_cast<BoxCollider*>(other);
	if(boxA && boxB) {
		return Collision::BoxToBox(boxA, boxB);
	}

	SphereCollider* sphereA = dynamic_cast<SphereCollider*>(this);
	SphereCollider* sphereB = dynamic_cast<SphereCollider*>(other);
	if(sphereA && sphereB) {
		return Collision::SphereToSphere(sphereA, sphereB);
	}

	if(boxA && sphereB) {
		return Collision::BoxToSphere(boxA, sphereB);
	}

	if(sphereA && boxB) {
		return Collision::BoxToSphere(boxB, sphereA);
	}

	return false;
}

void BaseCollider::UpdateMatrix() {
	transform_.UpdateMatrix();
	transform_.matTransform *= gameObject_->GetMatTransform();
}

Vec3 BaseCollider::GetPosition() const {
	return Mat4::Transform({}, transform_.matTransform);
}


#pragma region Collision


bool Collision::BoxToBox(BoxCollider* a, BoxCollider* b) {
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

bool Collision::BoxToSphere(BoxCollider* box, SphereCollider* sphere) {
	/// OBBの逆行列
	Matrix4x4 inverseObbWorldMatrix = box->transform_.matTransform.Inverse();
	/// Sphereの座標
	Vec3 spherePosition = sphere->GetPosition();
	/// Sphereの座標をOBBのLocal空間へ変換
	Vec3 obbLocalPosition = Mat4::Transform(spherePosition, inverseObbWorldMatrix);
	/// OBBのLocal空間内のAABB
	Vec3 min, max;
	min = /*box->GetPosition() */-box->GetSize();
	max = /*box->GetPosition() */+box->GetSize();


	/// AABBの最近接点を求める
	Vec3 closestPoint{
		std::clamp(obbLocalPosition.x, min.x, max.x),
		std::clamp(obbLocalPosition.y, min.y, max.y),
		std::clamp(obbLocalPosition.z, min.z, max.z)
	};

	float distance = Vec3::Length(closestPoint - obbLocalPosition);
	if(distance <= sphere->GetRadius()) {
		return true;
	}

	return false;
}

bool Collision::SphereToSphere(SphereCollider* a, SphereCollider* b) {
	Vec3 distance = a->GetPosition() - b->GetPosition();
	float length = distance.Len();

	///- 二点間の距離が二つの球の半径より小さければtrue
	if(length < a->GetRadius() + b->GetRadius()) {
		return true;
	}

	return false;
}

bool Collision::BoxToSegment(BoxCollider* box, const Vec3& start, const Vec3& end) {

	///- OBBの逆行列
	Matrix4x4 inverseObbWorldMatrix = box->GetObbMatTransform().Inverse();

	///- OBBのLocal空間に変換したAABB
	Vec3 localMin = -box->GetSize();
	Vec3 localMax = +box->GetSize();

	///- OBBのLocal空間に変換したSegment
	Vec3 origin = Mat4::Transform(start, inverseObbWorldMatrix);
	Vec3 diff = Mat4::Transform(end - start, inverseObbWorldMatrix);



	Vec3 aabbMin = (localMin - origin) / diff;
	Vec3 aabbMax = (localMax - origin) / diff;

	Vec3 nearPoint = {
		std::min(aabbMin.x, aabbMax.x),
		std::min(aabbMin.y, aabbMax.y),
		std::min(aabbMin.z, aabbMax.z)
	};

	Vec3 farPoint = {
		std::max(aabbMin.x, aabbMax.x),
		std::max(aabbMin.y, aabbMax.y),
		std::max(aabbMin.z, aabbMax.z)
	};

	float tmin = std::max({ nearPoint.x, nearPoint.y, nearPoint.z });
	float tmax = std::min({ farPoint.x, farPoint.y, farPoint.z });

	ImGui::Begin("box to segment");
	ImGui::DragFloat3("origin", &origin.x);
	ImGui::DragFloat3("diff  ", &diff.x);
	ImGui::Spacing();
	ImGui::DragFloat3("aabb min", &aabbMin.x);
	ImGui::DragFloat3("aabb max", &aabbMax.x);
	ImGui::Spacing();
	ImGui::DragFloat3("near pos", &nearPoint.x);
	ImGui::DragFloat3("far  pos", &farPoint.x);
	ImGui::Spacing();
	ImGui::DragFloat("tmin", &tmin);
	ImGui::DragFloat("tmax", &tmax);
	ImGui::End();

	///- Segment用の制限
	if(1.0f < tmin || tmax < 0.0f) {
		return false;
	}

	if(tmin <= tmax) {
		return true;
	}

	return false;
}


#pragma endregion Collision
