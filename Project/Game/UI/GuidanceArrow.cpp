#include"GuidanceArrow.h"
/// std
#include <numbers>

#include"Objects/Ground/Ground.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"

#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/time.h"

#include"HormingFunction/Horming.h"
#include"Easing/EasingFunction.h"

void GuidanceArrow::Initialize() {
	Model* model = ModelManager::Load("playerInGame");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -(Ground::groundScale_ + 3);

	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	pTransform_->SetParent(&pivot_);
}

void GuidanceArrow::Update() {

	// 距離と方向を計算
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
	 pBoss_->GetPosition(),pPlayer_->GetPosition(), Ground::groundScale_ + 1.0f);

	// 現在の回転をオイラー角に変換
	Vec3 euler = QuaternionToEulerAngles(pPlayer_->GetPivot()->quaternion);

	// プレイヤーの方向を向くための回転を計算
	Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

	// 現在の回転
	Quaternion currentRotation =pivot_.quaternion;

	// 回転をスムーズに補間 (Slerpを使用)
	float rotationSpeed = 6.0f; // 回転速度、必要に応じて調整
	Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ 2.0f * Time::DeltaTime(), 0, 0 });

	// 回転を更新
	pivot_.quaternion=(interpolatedRotation*move);
	//pBoss_->SetPivotSubtraction(move); // 移動もスムーズに

	pivot_.UpdateMatrix();

}

void GuidanceArrow::Debug() {



}

void GuidanceArrow::SetBoss(Boss* boss) {
	pBoss_ = boss;
}
void GuidanceArrow::SetPlayer(Player* player) {
	pPlayer_ = player;
}