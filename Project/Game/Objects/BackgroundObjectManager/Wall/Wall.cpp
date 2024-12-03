#include "Wall.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/BoxCollider.h"

Wall::Wall() {
	CreateTag(this);
}

Wall::~Wall() {}

void Wall::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(GetTag());

	Model*       collierModel = ModelManager::Load(GetTag());
	BoxCollider* collider     = AddComponent<BoxCollider>(collierModel);
}

void Wall::Update() {

}

