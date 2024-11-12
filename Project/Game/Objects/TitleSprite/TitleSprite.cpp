#include "TitleSprite.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

TitleSprite::TitleSprite() {
    CreateTag(this);
}

TitleSprite::~TitleSprite() {}

void TitleSprite::Initialize() {

    spriteRenderer_ = AddComponent<SpriteRenderer>();
    spriteRenderer_->SetTexture("TitleSprite.png");


    /// 値の保存と読み込み
    VariableManager* vm = VariableManager::GetInstance();
    const std::string& groupName = GetTag();

    vm->AddValue(groupName, "position", pTransform_->position);
    vm->AddValue(groupName, "scale",    pTransform_->scale);

    vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

    ApplyVariables();
}

void TitleSprite::Update() {
    ApplyVariables();

}

void TitleSprite::ApplyVariables() {
    VariableManager* vm = VariableManager::GetInstance();
    const std::string& groupName = GetTag();

    pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
    pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

