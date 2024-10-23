#include "BossName.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

BossName::BossName() {
    CreateTag(this);
}

BossName::~BossName() {}

void BossName::Initialize() {
    auto mr = AddComponent<MeshRenderer>();
    mr->SetModel("gageName");
    mr->SetMaterial("bossHpGageName");

    pTransform_->position.x = 1.0f;

}

void BossName::Update() {

}
