#include "BaseBuilding.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/Collider/SphereCollider.h>

#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
#include"Math/Random.h"

//object
#include"FrameManager/Time.h"


void BaseBuilding::Initialize() {

	 model_= ModelManager::Load("TestObject");

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model_);
	 auto collider = AddComponent<BoxCollider>(model_);
	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->position = { 0,0,-10.8f };//ポジション
	pTransform_->rotate = { -1.5f,0,0 };//回転
	pTransform_->scale = { 1.0f,0.1f,1.0f };//スケール
	scaleMax_ = 1.0f;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();

	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

}

void BaseBuilding::Update() {
	//成長
	growTime_ += Time::DeltaTime();
	GrowForTime(0.2f, 2.0f);

	//吸われる処理
	if (isSlurp_) {
		Vec3 euler = QuaternionToEulerAngles(pivot_.quaternion);

		// プレイヤーの方向を向くための回転を計算
		Quaternion inter = ToQuaternion({ euler.x, euler.y, -slurpDirection_ });

		// ホーミング移動のスピードを設定
		Quaternion move = ToQuaternion({ 0.01f, 0, 0 });

		// 回転を更新
    	pivot_.quaternion=inter;
		pivot_.quaternion*=move*Time::DeltaTime();

	}
	
	//ピボット更新
	pivot_.UpdateMatrix();
}

void BaseBuilding::Debug() {
	
}

void BaseBuilding::GrowForTime(const float& par,const float&second ) {

	//割合によるインクる面とする値を決める
	float incrementSize = scaleMax_ * par;

	if (growTime_ >= second) {//毎秒
		// 現在のスケール値に増加分を追加s
		pTransform_->scale.y += incrementSize;
	// スケールが最大値を超えないように制限
		if (pTransform_->scale.y > scaleMax_) {
			pTransform_->scale.y =scaleMax_;
		}
		growTime_ = 0.0f;
	}
}

void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	//当たったら用済み
	if (dynamic_cast<Player*>(collision)) {
		isDeath_ = true;
		
	}

	//当たったら用済み
	if (dynamic_cast<Boss*>(collision)&&isSlurp_) {
		isDeath_ = true;

	}
}

Vec3 BaseBuilding::QuaternionToEulerAngles(const Quaternion& q) {
	Vec3 euler;

	// X軸（Roll）の計算
	float sinX = 2.0f * (q.y * q.z - q.x * q.w);
	float absSinX = std::abs(sinX);
	const float epsilon = 0.001f;

	// 特定の範囲内でsinXをゼロにする（計算の安定性のため）
	if (absSinX < epsilon) {
		sinX = 0.0f;
	}

	euler.x = std::asin(-sinX);  // Roll角度（X軸回転）

	// X軸のcosを計算
	float cosX = std::cos(euler.x);

	// Y軸（Pitch）の計算
	float sinY = (2.0f * (q.x * q.z + q.y * q.w)) / cosX;
	float cosY = (2.0f * (q.w * q.w + q.z * q.z) - 1.0f) / cosX;
	euler.y = std::atan2(sinY, cosY);  // Pitch角度（Y軸回転）

	// Z軸（Yaw）の計算
	float sinZ = (2.0f * (q.x * q.y + q.z * q.w)) / cosX;
	float cosZ = (2.0f * (q.w * q.w + q.y * q.y) - 1.0f) / cosX;
	euler.z = std::atan2(sinZ, cosZ);  // Yaw角度（Z軸回転）

	return euler;
}

//Vector3からQuaternion変換
Quaternion BaseBuilding::ToQuaternion(const Vec3& eulerAngles) {

	float halfRoll = eulerAngles.z * 0.5f;   // Roll
	float halfPitch = eulerAngles.x * 0.5f;  // Pitch
	float halfYaw = eulerAngles.y * 0.5f;    // Yaw

	float cosRoll = std::cos(halfRoll);
	float sinRoll = std::sin(halfRoll);
	float cosPitch = std::cos(halfPitch);
	float sinPitch = std::sin(halfPitch);
	float cosYaw = std::cos(halfYaw);
	float sinYaw = std::sin(halfYaw);

	Quaternion q;
	q.x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw; // x
	q.y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw; // y
	q.z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw; // z
	q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw; // w

	return Quaternion::Normalize(q); // 正規化して返す
}
