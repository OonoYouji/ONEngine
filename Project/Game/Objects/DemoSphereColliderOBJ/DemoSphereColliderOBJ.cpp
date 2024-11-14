#include "DemoSphereColliderOBJ.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/Collider/SphereCollider.h"


DemoSphereColliderOBJ::DemoSphereColliderOBJ() {
	CreateTag(this);
}

DemoSphereColliderOBJ::~DemoSphereColliderOBJ() {}

void DemoSphereColliderOBJ::Initialize() {
	Model* model = ModelManager::Load("axis");
	SphereCollider* sc = AddComponent<SphereCollider>(model);
}

void DemoSphereColliderOBJ::Update() {

}

