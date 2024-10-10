#include "Boss.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"


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
	/*ChangeState(std::make_unique<>(this));*/
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;
	
	////ペアレント
	/*pTransform_->SetParent(&pivot_);*/

	//pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Boss::Update() {
	
}


void Boss::Debug() {

}

//void Boss::ChangeState(std::unique_ptr<BaseBossBehavior>behavior) {
//	//引数で受け取った状態を次の状態としてセット
//	behavior_ = std::move(behavior);
//}
