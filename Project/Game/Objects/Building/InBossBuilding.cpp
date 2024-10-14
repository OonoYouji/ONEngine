#include"InBossBuilding.h"
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
void InBossBuilding::Initialize() {
	
}
//更新
void InBossBuilding::Update() {
	
}
//デバッグ
void InBossBuilding::Debug() {
	if (ImGui::TreeNode("name")) {
	
		ImGui::TreePop();
	}
}

void InBossBuilding::SetModel(Model* model) {
	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model);
}

void InBossBuilding::SetBoss(Boss* boss) {
	pBoss_ = boss;
	pTransform_->SetParent(pBoss_->GetParent());//取るね――ドペアレント

}
