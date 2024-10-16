#include "BossAnimation.h"

BossAnimation::BossAnimation() {
    CreateTag(this);
}

BossAnimation::~BossAnimation() {}

void BossAnimation::Initialize() {

    modelNameArray_ = {
        "bossMainBody",
        "bossHead",
        "bossTubu"
    };


    bossPartsArray_.resize(BOSS_PARTS_COUNT);
    for(int i = 0; i < BOSS_PARTS_COUNT; ++i) {
        bossPartsArray_[i] = new BossParts;
        bossPartsArray_[i]->SetModelName(modelNameArray_[i]);
        bossPartsArray_[i]->Initialize();
        bossPartsArray_[i]->SetParent(pTransform_);
        bossPartsArray_[i]->SetName(modelNameArray_[i]);
    }


    /// 動きの定義を作成
    animationUpdateFunction_.resize(BOSS_ANIMATION_COUNT   );

    animationUpdateFunction_[BOSS_ANIMATION_NONE] = [&]() {
        BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
        BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];

        head->SetPosition({ 0, 0, -4.0f });
        tubu->SetPosition({ 0, 0, -2.5f });
    };


}

void BossAnimation::Update() {

    animationUpdateFunction_[currentAnimationIndex_]();
}
