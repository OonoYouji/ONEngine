#include"BossVacuum.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>
//obj
#include "Objects/Boss/Boss.h"

void BossTubu::Initialize() {
	Model* model = ModelManager::Load("bossTubu");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
}
void BossTubu::Update() {

}
void BossTubu::Debug() {

}

void BossTubu::SetBoss(Boss* boss) {
	pBoss_ = boss;
	pTransform_->SetParent(pBoss_->GetTransform());
}
///////////////////////////////////////////////////////////////////////////////////////////
// Bosshead
////////////////////////////////////////////////////////////////////////////////////////////

void BossHead::Initialize() {
	Model* model = ModelManager::Load("bossHead");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
}
void BossHead::Update() {

}
void BossHead::Debug() {

}

void BossHead::SetBossTube(BossTubu* bossTube) {
	pBossTube_ = bossTube;
	pTransform_->SetParent(pBossTube_->GetTransform());
}
