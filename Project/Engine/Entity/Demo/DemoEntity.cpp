#include "DemoEntity.h"

#include <numbers>

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/Component/Transform/Transform.h"

void DemoEntity::Initialize() {

	Line3DRenderer* renderer = AddComponent<Line3DRenderer>();
	renderer->SetLine(
		Vector3(0.0f, 0.0f, 0.0f), 
		Vector3(0.0f, 0.0f, 1.0f), 
		Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	);

	renderer->SetLine(
		Vector3(0.0f, 0.0f, 0.0f), 
		Vector3(1.0f, 1.0f, 0.0f), 
		Vector4(0.0f, 1.0f, 0.0f, 1.0f)
	);


}

void DemoEntity::Update() {

	//transform_->rotate.y += 0.01f;

	transform_->Update();
}
