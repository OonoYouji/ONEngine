#include "Home.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"


Home::Home() {
    CreateTag(this);
}

Home::~Home() {}

void Home::Initialize() {

    MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
    meshRenderer->SetModel("Home");

    pTransform_->position = { 10.9f, 22.6f, -6.8f };

}

void Home::Update() {

}

