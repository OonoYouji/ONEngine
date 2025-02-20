#include "Block.h"

#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"


Block::Block() {}
Block::~Block() {}

void Block::Initialize() {

	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/primitive/cube.obj");
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");
}

void Block::Update() {
	
	MeshRenderer* renderer = GetComponent<MeshRenderer>();
	renderer->SetColor(type_ == static_cast<int>(BlockType::black) ? Vector4(0, 0, 0, 1) : Vector4::kWhite);

}
