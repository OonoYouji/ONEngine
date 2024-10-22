#include"InTornadoEnemy.h"
#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
#include"Math/Random.h"

#include<algorithm>

//object
#include"FrameManager/Time.h"
#include"Objects/Boss/Boss.h"

//初期化
void InTornadoEnemy::Initialize() {
	speed_ = Random::Float(4.0f, 4.5f);//回転スピード
	radius_.x = Random::Float(0.3f, 0.5f);//半径
	radius_.y = Random::Float(0.3f, 0.5f);//半径


	pTransform_->rotate = { -1.5f,0,0 };//回転
	pTransform_->scale = { 0.7f,0.7f,0.7f };
	maxDebuf_ = 3.0f;
	auto model = ModelManager::Load("Enemy");
	auto mesh_ = AddComponent<MeshRenderer>();
	mesh_->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
}
//更新
void InTornadoEnemy::Update() {
	// 回転速度を加算
	float debuffRatio = 1.0f - (pTornado_->GetDebufParm() / maxDebuf_);  // 最大デバフ値で割って割合を計算
	debuffRatio = max(0.0f, debuffRatio);  

	theta_ += ((speed_ * debuffRatio) / pTornado_->GetTransform()->scale.x) * Time::TimeRateDeltaTime();
	phi_ += ((speed_ * debuffRatio) / pTornado_->GetTransform()->scale.y) * Time::TimeRateDeltaTime();
	// 楕円の長軸と短軸
	float longAxis = pTornado_->GetTransform()->scale.x + radius_.x;  // 長軸 (x方向の半径)
	float shortAxis = pTornado_->GetTransform()->scale.y + radius_.x; // 短軸 (y方向の半径)

	// 楕円座標を計算
	float x = longAxis * sin(theta_);
	float y = shortAxis * cos(phi_); 
	pTransform_->position = { x,y,2 };
}
//デバッグ
void InTornadoEnemy::Debug() {
	if (ImGui::TreeNode("RotateOfset")) {
		ImGui::DragFloat("rotateXAngle", &ofsetX, 0.01f);
		ImGui::DragFloat("rotateYAngle", &ofsetY, 0.01f);
		ImGui::TreePop();
	}
}

//トルネードセット
void InTornadoEnemy::SetTornado(Tornado* tornade) {
	pTornado_ = tornade;
	pTransform_->SetParent(pTornado_->GetParent());//取るね――ドペアレント

}
//死亡フラグ
void InTornadoEnemy::SetIsDeath(bool is) {
	isDeath_ = is;
	ParentCancel(true);
}


void InTornadoEnemy::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<Boss*>(collision) && !isDeath_) {
		SetIsDeath(true);
	}
}
