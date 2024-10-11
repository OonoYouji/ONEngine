#include"InTornadoBuilding.h"
#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
#include"Math/Random.h"

//object
#include"FrameManager/Time.h"

//初期化
void InTornadoBuilding::Initialize() {
	speed_ = Random::Float(4.0f, 5.0f);//回転スピード
	radius_ = Random::Float(-0.5f, 0.5f);//半径
	pTransform_->scale = { 0.2f,0.2f,0.2f };
	ofsetX = -0.14f;
	ofsetY = -1.10f;
}
//更新
void InTornadoBuilding::Update() {
	//回転速度を加算
	theta_ += (speed_ / pTornado_->GetTransform()->scale.x) * Time::DeltaTime();
	phi_ += (speed_ / pTornado_->GetTransform()->scale.x) * Time::DeltaTime();
	//球面座標を計算
	float x = (pTornado_->GetTransform()->scale.x + radius_) * sin(theta_);
	float y = (pTornado_->GetTransform()->scale.y + radius_) * cos(theta_);
	/*float z = radius * cos(theta_);*/
	pTransform_->position = { x,y,-2 };
}
//デバッグ
void InTornadoBuilding::Debug() {
	if (ImGui::TreeNode("RotateOfset")) {
		ImGui::DragFloat("rotateXAngle", &ofsetX, 0.01f);
		ImGui::DragFloat("rotateYAngle", &ofsetY, 0.01f);
		ImGui::TreePop();
	}
}

void InTornadoBuilding::SetModel(Model* model) {
	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model);
}

void InTornadoBuilding::SetTornado(Tornado* tornade) {
	pTornado_ = tornade;
	pTransform_->SetParent(pTornado_->GetParent());//取るね――ドペアレント

}
