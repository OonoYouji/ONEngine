#include "DemoEntity.h"

#include <numbers>

/// engine
#include "Engine/ECS/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/RendererComponents/Primitive/Line3DRenderer.h"

void DemoEntity::Initialize() {

	SpriteRenderer* renderer = AddComponent<SpriteRenderer>();
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

	transform_->position.y = 3;


}

void DemoEntity::Update() {

	//transform_->rotate.y += 0.01f;

	transform_->Update();
}
