#pragma once

/// engine
#include "Engine/Asset/Collection/GraphicsResourceCollection.h"

/// ////////////////////////////////////////////
/// primitive mesh creator
/// ////////////////////////////////////////////
class PrimitiveMeshCreator final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PrimitiveMeshCreator(GraphicsResourceCollection* _graphicsResourceCollection, class DxDevice* _dxDevice);
	~PrimitiveMeshCreator();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	GraphicsResourceCollection* pGraphicsResourceCollection_ = nullptr;
	class DxDevice* pDxDevice_ = nullptr;

public:


	void CreateCube();
	void CreateSphere();
	void CreateCylinder();
	void CreateCone();
	void CreateTorus();
	void CreatePlane();
};

