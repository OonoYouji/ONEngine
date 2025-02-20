#include "DemoEntity.h"

#include <numbers>

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/Component/Transform/Transform.h"

void DemoEntity::Initialize() {

	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/primitive/sphere.obj");
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

	transform_->position.y = 3;


}

void DemoEntity::Update() {

	transform_->rotate.y += 0.01f;

	transform_->Update();
}
