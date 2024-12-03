#include "DemoBoxColliderOBJ.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/Collider/BoxCollider.h"


DemoBoxColliderOBJ::DemoBoxColliderOBJ() {
    CreateTag(this);
}

DemoBoxColliderOBJ::~DemoBoxColliderOBJ() {}

void DemoBoxColliderOBJ::Initialize() {
	Model* model = ModelManager::Load("axis");
	BoxCollider* bc = AddComponent<BoxCollider>(model);

}

void DemoBoxColliderOBJ::Update() {

}

