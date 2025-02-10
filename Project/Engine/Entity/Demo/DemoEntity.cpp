#include "DemoEntity.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/Transform/Transform.h"

void DemoEntity::Initialize() {
	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshId(0);

}

void DemoEntity::Update() {

}
