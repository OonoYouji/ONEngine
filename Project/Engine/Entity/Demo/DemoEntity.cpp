#include "DemoEntity.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/Transform/Transform.h"

void DemoEntity::Initialize() {
	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshId(0);

	transform_->scale  = Vector3::kOne;
	transform_->rotate = Vector3::kZero;
}

void DemoEntity::Update() {

	transform_->rotate.y += 0.01f;

	transform_->Update();
}
