#include "GaugeName.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

GaugeName::GaugeName() {
    CreateTag(this);
}

GaugeName::~GaugeName() {}

void GaugeName::Initialize() {
    auto mr = AddComponent<MeshRenderer>();
    mr->SetModel("gageName");


    pTransform_->position.x = -1.0f;
}

void GaugeName::Update() {

}
