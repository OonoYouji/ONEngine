#include "PlayerMesh.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

PlayerMesh::PlayerMesh() {
	CreateTag(this);
}

PlayerMesh::~PlayerMesh() {}

void PlayerMesh::Initialize() {

	/// ===================================================
	/// setting
	/// ===================================================

	/// component setting
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Player");


	pTransform_->position.y = 1.0f;

}

void PlayerMesh::Update() {}

void PlayerMesh::Debug() {}


