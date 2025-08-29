#include "PrimitiveMeshCreator.h"

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"

PrimitiveMeshCreator::PrimitiveMeshCreator(GraphicsResourceCollection* _graphicsResourceCollection, DxDevice* _dxDevice)
	: pGraphicsResourceCollection_(_graphicsResourceCollection), pDxDevice_(_dxDevice) {

	CreatePlane();
}

PrimitiveMeshCreator::~PrimitiveMeshCreator() {}


void PrimitiveMeshCreator::CreateCube() {
	//std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	//mesh->SetVertices(
	//	{
	//	
	//	}
	//);
}

void PrimitiveMeshCreator::CreateSphere() {
	// TODO : 実装
}

void PrimitiveMeshCreator::CreateCylinder() {
	// TODO : 実装
}

void PrimitiveMeshCreator::CreateCone() {
	// TODO : 実装
}

void PrimitiveMeshCreator::CreateTorus() {
	// TODO : 実装
}

void PrimitiveMeshCreator::CreatePlane() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->SetVertices(
		{
			Mesh::VertexData({ -0.5f, 0.0f, +0.5f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
			Mesh::VertexData({ +0.5f, 0.0f, +0.5f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }),
			Mesh::VertexData({ -0.5f, 0.0f, -0.5f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
			Mesh::VertexData({ +0.5f, 0.0f, -0.5f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f })
		}
	);

	mesh->SetIndices(
		{
			0, 1, 2,
			1, 3, 2
		}
	);

	mesh->CreateBuffer(pDxDevice_);
	Model model;
	model.AddMesh(std::move(mesh));

	pGraphicsResourceCollection_->AddModel("plane", std::move(model));
}

