#include "DemoEntity.h"

#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"

void DemoEntity::Initialize() {

	//SpriteRenderer* renderer = AddComponent<SpriteRenderer>();
	//renderer->SetTexturePath("Packages/Textures/uvChecker.png");

	//transform_->position.y = 3;

	MeshRenderer* mesh = AddComponent<MeshRenderer>();
	mesh->SetMeshPath("Assets/Models/primitive/sphere.obj");
	mesh->SetTexturePath("Packages/Textures/white.png");
}

void DemoEntity::Update() {

	//transform_->rotate.y += 0.01f;

	transform_->Update();
}
