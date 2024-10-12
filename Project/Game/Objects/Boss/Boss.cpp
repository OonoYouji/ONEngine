#include "Boss.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

//std
#include <algorithm>
#include<numbers>
#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/Time.h"
//obj
#include"Objects/Player/Player.h"


void Boss::Initialize() {
	Model* model = ModelManager::Load("axis");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();
	ChangeState(std::make_unique<BossChasePlayer>(this));
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	SpeedParamater_ = 0.01f;
	pTransform_->position.z = -12;
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////	
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;
	///////////////////////////////////////////////////////////////////////////////////////////
	//	ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////	
	pTransform_->SetParent(&pivot_);
	//行列更新
	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Boss::Update() {
	//振る舞い更新
	behavior_->Update();

	pivot_.UpdateMatrix();
}

//通常更新
void Boss::RootInit() {

}
void Boss::RootUpdate() {

}
//ストーカー
void Boss::ChaseInit() {

}
void Boss::ChaseUpdate(const float& speed) {
	// プレイヤーの位置を取得
	Vec3 playerPosition = pPlayer_->GetPosition();

	// ボスの現在の位置を取得
	Vec3 bossPosition = GetPosition();

	// ボスとプレイヤーの3次元座標を使って球面座標に変換
	float latitude1 = std::asin(bossPosition.y / Vector3::Length(bossPosition)); // ボスの緯度
	float longitude1 = std::atan2(bossPosition.z, bossPosition.x); // ボスの経度

	float latitude2 = std::asin(playerPosition.y / Vector3::Length(playerPosition)); // プレイヤーの緯度
	float longitude2 = std::atan2(playerPosition.z, playerPosition.x); // プレイヤーの経度

	// 中心角を計算
	float deltaSigma = std::acos(
		std::sin(latitude1) * std::sin(latitude2) +
		std::cos(latitude1) * std::cos(latitude2) * std::cos(longitude2 - longitude1)
	);

	// 球面上の距離を計算
	float sphereRadius = 12.0f; // 半径
	float distance = sphereRadius * deltaSigma;

	// 方位角を計算
	float deltaLon = longitude2 - longitude1;
	float y = std::sin(deltaLon) * std::cos(latitude2);
	float x = std::cos(latitude1) * std::sin(latitude2) - std::sin(latitude1) * std::cos(latitude2) * std::cos(deltaLon);
	float direction = std::atan2(y, x);

	// 現在の回転をオイラー角に変換
	Vec3 euler = QuaternionToEulerAngles(pivot_.quaternion);

	// プレイヤーの方向を向くための回転を計算
	Quaternion inter = ToQuaternion({ euler.x, euler.y, -direction });

	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ speed, 0, 0 });

	// 回転を更新
	pivot_.quaternion = inter;
	pivot_.quaternion *= move;
}

void Boss::Debug() {
	if (ImGui::TreeNode("Paramater")) {
		ImGui::DragFloat("ChaseSpeedMax", &SpeedParamater_, 0.001f);
		ImGui::TreePop();
	}
}

void Boss::ChangeState(std::unique_ptr<BaseBossBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}

void Boss::SetPlayer(Player*player) {
	pPlayer_ = player;
}


Vec3 Boss::QuaternionToEulerAngles(const Quaternion& q) {
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
Quaternion Boss::ToQuaternion(const Vec3& eulerAngles) {

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

