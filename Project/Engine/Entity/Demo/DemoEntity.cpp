#include "DemoEntity.h"

#include <numbers>

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/Transform/Transform.h"

void DemoEntity::Initialize() {
	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/primitive/cylinder.obj");

	transform_->scale  = Vector3::kOne;
	transform_->rotate = Vector3::kZero;

	//transform_->rotate.x = std::numbers::pi_v<float> / 4.0f;
	transform_->rotate.y = std::numbers::pi_v<float> / 2.0f;

}

void DemoEntity::Update() {

	transform_->rotate.y += 0.01f;

	transform_->Update();
}
